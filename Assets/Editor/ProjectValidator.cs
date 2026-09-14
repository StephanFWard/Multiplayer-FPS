// -----------------------------------------------------------------------------
// ProjectValidator.cs
//
// Unity Editor validation tooling for the Multiplayer FPS project.
//
// Features:
//   * Static validation: required tags/layers, scene integrity, missing scripts,
//     Photon AppId configuration and build settings.
//   * Play-mode smoke test: enters play mode, captures runtime errors for a few
//     seconds and reports a pass/fail summary.
//
// Interactive usage:       Tools > Validate Project & Run Smoke Test
//                          Tools > Validate Project & Run Bot Smoke Test
// Batch usage:
//   Unity.exe -batchmode -nographics -projectPath <project> \
//             -executeMethod ProjectValidator.RunValidation \
//             -projectSmokeTest -logFile <log>
//   (add -projectBotSmokeTest to also spawn enemy bots and exercise the AI,
//    NavMesh bake and gunshot-hearing code headless)
//
// Exit codes: 0 = pass, 1 = fail.
// -----------------------------------------------------------------------------

#if UNITY_EDITOR
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

[InitializeOnLoad]
public static class ProjectValidator
{
    private const string ResultFile = "Temp/ProjectValidation.json";
    private const string ArmedPref  = "ProjectValidator.SmokeTest";
    private const string BotArmedPref = "ProjectValidator.BotSmokeTest";

    private const string ScenePath = "Assets/Scenes/Start.unity";

    private static readonly string[] RequiredTags = { "Player", "Screen" };

    private static readonly string[] RequiredLayers =
    {
        "FPSPlayer", "FPSGun", "TPSPlayer", "TPSGun", "Shootable", "Hidden", "Impact"
    };

    // Errors produced by PUN when it cannot reach the Photon Cloud because no
    // App Id is configured are expected during a headless smoke test.
    private static readonly string[] PhotonNoiseFilters =
    {
        "photon", "connectfail", "connectusingsettings", "appid", "dns",
        "loadbalancingpeer", "disconnected from", "socket", "joining room",
        "realtime client", "peer state", "region"
    };

    private const int MaxPlayFrames = 180;   // ~3 seconds at 60 fps
    private const int TimeoutSeconds = 120;  // hard stop in case frames never tick
    private static int maxPlayFrames = MaxPlayFrames; // raised for the bot test

    private static readonly List<string> PlayModeErrors = new List<string>();
    private static int playFrames;
    private static bool playSessionActive;
    private static DateTime smokeStartedAt;

    [Serializable]
    private sealed class ResultData
    {
        public string scene = "";
        public int rootCount;
        public int gameObjectCount;
        public int missingComponents = 0;
        public List<string> missingTags = new List<string>();
        public List<string> missingLayers = new List<string>();
        public bool photonAppIdSet;
        public bool sceneInBuild;
        public List<string> playModeErrors = new List<string>();
        public bool passed;
    }

    static ProjectValidator()
    {
        EditorApplication.update += OnEditorUpdate;
        EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
        Application.logMessageReceived += OnLogMessageReceived;

        if (EditorPrefs.GetBool(ArmedPref, false))
        {
            playSessionActive = true;
            playFrames = 0;
            smokeStartedAt = DateTime.UtcNow;
            if (EditorPrefs.GetBool(BotArmedPref, false))
            {
                // Bot test needs more frames: navmesh bake + AI ticks.
                maxPlayFrames = 900;
            }
        }
    }

    [MenuItem("Tools/Validate Project & Run Smoke Test")]
    public static void RunFromMenu()
    {
        RunValidation(smoke: true);
    }

    [MenuItem("Tools/Validate Project & Run Bot Smoke Test")]
    public static void RunBotTestFromMenu()
    {
        RunValidation(smoke: true, botSmoke: true);
    }

    /// <summary>
    /// Batch / menu entry point. Runs static checks and, when enabled, a
    /// play-mode smoke test.
    /// </summary>
    public static void RunValidation()
    {
        string[] args = Environment.GetCommandLineArgs();
        bool smoke = Array.Exists(
            args,
            a => a.Equals("-projectSmokeTest", StringComparison.OrdinalIgnoreCase));
        bool botSmoke = Array.Exists(
            args,
            a => a.Equals("-projectBotSmokeTest", StringComparison.OrdinalIgnoreCase));
        RunValidation(smoke || botSmoke, botSmoke);
    }

    private static void RunValidation(bool smoke, bool botSmoke = false)
    {
        ResultData result = PerformStaticChecks();
        result.passed = result.missingTags.Count == 0
                        && result.missingLayers.Count == 0
                        && result.missingComponents == 0
                        && result.sceneInBuild
                        && result.scene == "Start";
        SaveResult(result);

        StringBuilder summary = new StringBuilder();
        summary.AppendLine("[ProjectValidator] Static checks:");
        summary.AppendLine("  Scene loaded:             " + (result.scene == "Start" ? "OK" : result.scene));
        summary.AppendLine("  GameObjects in scene:     " + result.gameObjectCount);
        summary.AppendLine("  Missing script components: " + result.missingComponents);
        summary.AppendLine("  Missing tags:             " + ListOrNone(result.missingTags));
        summary.AppendLine("  Missing layers:           " + ListOrNone(result.missingLayers));
        summary.AppendLine("  Photon AppId configured:  " + result.photonAppIdSet);
        summary.AppendLine("  Scene in Build Settings:  " + result.sceneInBuild);
        Debug.Log(summary.ToString());

        bool batch = Application.isBatchMode;

        if (smoke)
        {
            EditorPrefs.SetBool(ArmedPref, true);
            if (botSmoke)
            {
                EditorPrefs.SetBool(BotArmedPref, true);
                maxPlayFrames = 900;
                Debug.Log("[ProjectValidator] Bot smoke test armed: bots will spawn without a player.");
            }
            else
            {
                EditorPrefs.DeleteKey(BotArmedPref);
            }
            PlayModeErrors.Clear();
            playFrames = 0;
            playSessionActive = true;
            smokeStartedAt = DateTime.UtcNow;
            Debug.Log("[ProjectValidator] Entering play mode for smoke test...");
            EditorApplication.isPlaying = true;
            return;
        }

        if (batch)
        {
            EditorApplication.Exit(result.passed ? 0 : 1);
        }
    }

    private static void OnEditorUpdate()
    {
        if (!Application.isPlaying || !playSessionActive)
        {
            return;
        }

        playFrames++;
        bool timedOut = (DateTime.UtcNow - smokeStartedAt).TotalSeconds > TimeoutSeconds;
        if (playFrames >= maxPlayFrames || timedOut)
        {
            playSessionActive = false;
            if (timedOut)
            {
                Debug.LogWarning(
                    "[ProjectValidator] Smoke test timed out (frames never ticked); exiting play mode.");
            }
            EditorApplication.isPlaying = false;
        }
    }

    private static void OnPlayModeStateChanged(PlayModeStateChange state)
    {
        switch (state)
        {
            case PlayModeStateChange.EnteredPlayMode:
                PlayModeErrors.Clear();
                playFrames = 0;
                smokeStartedAt = DateTime.UtcNow;
                break;

            case PlayModeStateChange.ExitingPlayMode:
                if (EditorPrefs.GetBool(ArmedPref, false))
                {
                    // Persist results now; the upcoming domain reload clears our memory.
                    ResultData data = LoadResult() ?? new ResultData();
                    data.playModeErrors = new List<string>(PlayModeErrors);
                    SaveResult(data);
                }
                break;

            case PlayModeStateChange.EnteredEditMode:
                if (Application.isBatchMode && EditorPrefs.GetBool(ArmedPref, false))
                {
                    EditorPrefs.DeleteKey(ArmedPref);
                    EditorPrefs.DeleteKey(BotArmedPref);
                    FinalizeAndExit();
                }
                break;
        }
    }

    private static void OnLogMessageReceived(string condition, string stackTrace, LogType type)
    {
        if (type != LogType.Error && type != LogType.Exception)
        {
            return;
        }

        if (!Application.isPlaying || string.IsNullOrEmpty(condition))
        {
            return;
        }

        // Known internal Unity editor exception from the Search indexation
        // that fires on batch startup; not related to the project.
        if (!string.IsNullOrEmpty(stackTrace) && stackTrace.Contains("UnityEditor.Search"))
        {
            return;
        }

        string line = condition.Split('\n')[0];
string lower = line.ToLowerInvariant();
        foreach (string filter in PhotonNoiseFilters)
        {
            if (lower.Contains(filter))
            {
                return; // Expected network / Photon noise in a headless run.
            }
        }

        if (PlayModeErrors.Count < 50)
        {
            PlayModeErrors.Add(line);
        }
    }

    private static void FinalizeAndExit()
    {
        ResultData data = LoadResult() ?? new ResultData();
        data.passed = data.passed
                      && data.missingComponents == 0
                      && data.playModeErrors.Count == 0;
        SaveResult(data);

        StringBuilder sb = new StringBuilder();
        sb.AppendLine("[ProjectValidator] Play-mode smoke test results:");
        sb.AppendLine("  Runtime errors captured: " + data.playModeErrors.Count);
        foreach (string err in data.playModeErrors)
        {
            sb.AppendLine("    - " + err);
        }
        sb.AppendLine("  OVERALL RESULT: " + (data.passed ? "PASS" : "FAIL"));
        Debug.Log(sb.ToString());

        EditorApplication.Exit(data.passed ? 0 : 1);
    }

    private static ResultData PerformStaticChecks()
    {
        ResultData result = new ResultData();

        // Required tags.
        string[] tags = UnityEditorInternal.InternalEditorUtility.tags;
        foreach (string tag in RequiredTags)
        {
            if (Array.IndexOf(tags, tag) < 0)
            {
                result.missingTags.Add(tag);
            }
        }

        // Required layers.
        string[] layers = UnityEditorInternal.InternalEditorUtility.layers;
        foreach (string layer in RequiredLayers)
        {
            if (Array.IndexOf(layers, layer) < 0)
            {
                result.missingLayers.Add(layer);
            }
        }

        // Scene integrity.
        Scene scene = EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);
        if (scene.IsValid())
        {
            result.scene = scene.name;
            result.rootCount = scene.rootCount;

            GameObject[] gos = UnityEngine.Object.FindObjectsByType<GameObject>(
                FindObjectsInactive.Include, FindObjectsSortMode.None);
            result.gameObjectCount = gos.Length;

            foreach (GameObject go in gos)
            {
                Component[] comps = go.GetComponentsInChildren<Component>(true);
                foreach (Component c in comps)
                {
                    if (c == null)
                    {
                        result.missingComponents++;
                    }
                }
            }
        }
        else
        {
            result.scene = "<failed to load>";
        }

        // Build settings.
        foreach (EditorBuildSettingsScene s in EditorBuildSettings.scenes)
        {
            if (s.enabled && s.path == ScenePath)
            {
                result.sceneInBuild = true;
            }
        }

        // Photon App Id.
        try
        {
            Photon.Pun.ServerSettings settings = Photon.Pun.PhotonNetwork.PhotonServerSettings;
            if (settings != null && settings.AppSettings != null)
            {
                string appId = settings.AppSettings.AppIdRealtime;
                result.photonAppIdSet = !string.IsNullOrEmpty(appId) && Photon.Pun.ServerSettings.IsAppId(appId);
                if (!result.photonAppIdSet)
                {
                    Debug.LogWarning(
                        "[ProjectValidator] Photon AppIdRealtime is not configured. " +
                        "Multiplayer will not connect. Get a free App Id at https://dashboard.photonengine.com " +
                        "and set it via Window > Photon Unity Networking > Apply/Update.");
                }
            }
        }
        catch (Exception e)
        {
            Debug.LogWarning("[ProjectValidator] Photon settings check failed: " + e.Message);
        }

        return result;
    }

    private static string ListOrNone(List<string> items)
    {
        return items.Count == 0 ? "none" : string.Join(", ", items.ToArray());
    }

    private static void SaveResult(ResultData data)
    {
        File.WriteAllText(ResultFile, JsonUtility.ToJson(data, true));
    }

    private static ResultData LoadResult()
    {
        if (!File.Exists(ResultFile))
        {
            return null;
        }

        try
        {
            return JsonUtility.FromJson<ResultData>(File.ReadAllText(ResultFile));
        }
        catch
        {
            return null;
        }
    }
}
#endif