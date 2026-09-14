using System.Collections;
using System.Collections.Generic;
using Unity.AI.Navigation;
using UnityEngine;
using UnityEngine.AI;

/// <summary>
/// Spawns enemy bots (EnemyBot) across the city map.
///
/// This component bootstraps itself at play time via
/// RuntimeInitializeOnLoadMethod — no scene setup is required:
///
/// 1. It bakes a NavMesh over the "Enviroment" city map (animated doors are
///    excluded from the bake so doorways stay walkable; bots open them by
///    walking into the door triggers).
/// 2. It samples street / building-interior positions from the baked NavMesh.
/// 3. As soon as the local player has spawned (or the batch bot-smoke-test
///    flag is armed) it populates the map with bots and keeps the population
///    topped up as bots are killed.
/// </summary>
public class EnemySpawner : MonoBehaviour {

    /// <summary>Singleton instance created at play-time bootstrap.</summary>
    public static EnemySpawner Instance { get; private set; }

    /// <summary>
    /// Walkable ground-level points on the baked NavMesh, shared with bots
    /// for patrol destinations and spawn placement.
    /// </summary>
    public static readonly List<Vector3> WalkablePoints = new List<Vector3>();

    [Header("Population")]
    [Tooltip("Number of bots to keep alive on the map.")]
    [SerializeField]
    private int botCount = 6;
    [Tooltip("Seconds between respawn attempts while below the bot count.")]
    [SerializeField]
    private float respawnDelay = 6.0f;

    private static bool bootstrapped;
    private readonly List<EnemyBot> bots = new List<EnemyBot>();
    private bool activated;
    private bool navMeshReady;
    private float nextSpawnTime;

    /// <summary>
    /// True when the editor bot smoke test
    /// (ProjectValidator -projectBotSmokeTest) is armed; makes bots spawn
    /// even without a joined player so AI code can be validated headless in
    /// batch mode.
    /// </summary>
    public static bool BotSmokeTest {
        get {
#if UNITY_EDITOR
            return UnityEditor.EditorPrefs.GetBool("ProjectValidator.BotSmokeTest", false);
#else
            return false;
#endif
        }
    }

    /// <summary>
    /// Create the spawner automatically when play mode starts.
    /// </summary>
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
    static void Bootstrap() {
        if (bootstrapped) {
            return;
        }
        bootstrapped = true;
        GameObject spawnerObject = new GameObject("EnemySpawner");
        spawnerObject.AddComponent<EnemySpawner>();
    }

    IEnumerator Start() {
        Instance = this;
        yield return null; // Let the scene settle for a frame.
        yield return BuildNavMeshIfNeeded();
        WalkablePoints.Clear();
        WalkablePoints.AddRange(SampleWalkablePoints(Mathf.Max(botCount, 12)));
        navMeshReady = WalkablePoints.Count > 0;
        Debug.Log("[EnemySpawner] Ready with " + WalkablePoints.Count
                  + " walkable street/interior points; waiting for a player to spawn...");
    }

    /// <summary>
    /// Update activates bots once the NavMesh is ready and a player exists
    /// (or the bot smoke test is armed); it keeps the bot population topped
    /// up afterwards.
    /// </summary>
    void Update() {
        if (!navMeshReady) {
            return;
        }
        if (!activated) {
            bool playerPresent = GameObject.FindWithTag("Player") != null;
            if (playerPresent || BotSmokeTest) {
                activated = true;
                nextSpawnTime = 0.0f;
                int count = BotSmokeTest ? 2 : botCount;
                for (int i = 0; i < count; i++) {
                    SpawnBot();
                }
                Debug.Log("[EnemySpawner] Activated with " + bots.Count + " bots"
                          + (BotSmokeTest ? " (smoke test mode)." : "."));
            }
            return;
        }

        bots.RemoveAll(bot => bot == null);
        if (bots.Count < botCount && Time.time >= nextSpawnTime) {
            nextSpawnTime = Time.time + respawnDelay;
            SpawnBot();
        }
    }

    /// <summary>
    /// Called by a bot when it dies so the spawner can schedule a respawn.
    /// </summary>
    public void NotifyBotDied(EnemyBot bot) {
        bots.Remove(bot);
    }

    /// <summary>
    /// Spawn a single bot at a random walkable point. Public so the smoke
    /// test and future game modes can spawn bots directly.
    /// </summary>
    public EnemyBot SpawnBot() {
        Vector3 position = Vector3.zero;
        if (WalkablePoints.Count > 0) {
            position = WalkablePoints[Random.Range(0, WalkablePoints.Count)];
            NavMeshHit hit;
            if (NavMesh.SamplePosition(position, out hit, 5.0f, NavMesh.AllAreas)) {
                position = hit.position;
            }
        }
        if (position == Vector3.zero) {
            Debug.LogWarning("[EnemySpawner] No walkable spawn point available; bot skipped.");
            return null;
        }
        GameObject botObject = new GameObject("Enemy Bot");
        botObject.transform.position = position;
        botObject.transform.rotation = Quaternion.Euler(0.0f, Random.Range(0.0f, 360.0f), 0.0f);
        botObject.layer = LayerMask.NameToLayer("Shootable");
        EnemyBot bot = botObject.AddComponent<EnemyBot>();
        bots.Add(bot);
        return bot;
    }

    /// <summary>
    /// Bake a NavMesh over the city environment at runtime if the project
    /// does not already contain one. Tries progressively broader collection
    /// strategies so the bake works regardless of how the map colliders are
    /// organised, logging diagnostics along the way.
    /// </summary>
    IEnumerator BuildNavMeshIfNeeded() {
        if (NavMesh.CalculateTriangulation().vertices.Length > 0) {
            Debug.Log("[EnemySpawner] Existing NavMesh found; skipping runtime bake.");
            yield break;
        }
        GameObject environment = GameObject.Find("Enviroment");
        if (environment == null) {
            Debug.LogError("[EnemySpawner] Could not find the 'Enviroment' root object to bake a NavMesh on.");
            yield break;
        }

        // Exclude animated doors from the bake so doorways remain walkable;
        // bots open the doors by walking into their trigger volumes.
        DoorAnimation[] doors = Object.FindObjectsByType<DoorAnimation>(FindObjectsInactive.Exclude);
        foreach (DoorAnimation door in doors) {
            NavMeshModifier modifier = door.gameObject.AddComponent<NavMeshModifier>();
            modifier.ignoreFromBuild = true;
        }

        int colliderCount = 0;
        foreach (Collider collider in environment.GetComponentsInChildren<Collider>(false)) {
            colliderCount++;
        }
        int meshCount = 0;
        foreach (MeshFilter meshFilter in environment.GetComponentsInChildren<MeshFilter>(false)) {
            if (meshFilter.sharedMesh != null) {
                meshCount++;
            }
        }
        Debug.Log("[EnemySpawner] Environment contains " + colliderCount
                  + " colliders and " + meshCount + " meshes.");

        GameObject surfaceObject = new GameObject("Runtime NavMesh");
        surfaceObject.transform.SetParent(environment.transform, false);

        // Strategy 1: all physics colliders in the scene (most reliable).
        TryBake(surfaceObject, CollectObjects.All, NavMeshCollectGeometry.PhysicsColliders,
            "whole scene / physics colliders");
        if (HasNavMeshData()) {
            yield break;
        }
        yield return null;

        // Strategy 2: physics colliders under the environment.
        TryBake(surfaceObject, CollectObjects.Children, NavMeshCollectGeometry.PhysicsColliders,
            "children / physics colliders");
        if (HasNavMeshData()) {
            yield break;
        }
        yield return null;

        // Strategy 3: render meshes in the scene.
        TryBake(surfaceObject, CollectObjects.All, NavMeshCollectGeometry.RenderMeshes,
            "whole scene / render meshes");
        if (!HasNavMeshData()) {
            Debug.LogError("[EnemySpawner] All NavMesh bake strategies failed; bots cannot navigate.");
        }
    }

    /// <summary>Bake a NavMesh with the given collection settings.</summary>
    static void TryBake(GameObject surfaceObject, CollectObjects collectObjects,
        NavMeshCollectGeometry useGeometry, string label) {
        NavMeshSurface surface = surfaceObject.GetComponent<NavMeshSurface>();
        if (surface == null) {
            surface = surfaceObject.AddComponent<NavMeshSurface>();
        }
        surface.collectObjects = collectObjects;
        surface.useGeometry = useGeometry;

        Debug.Log("[EnemySpawner] Baking NavMesh (" + label + ")...");
        surface.BuildNavMesh();
        int vertices = NavMesh.CalculateTriangulation().vertices.Length;
        if (vertices > 0) {
            Debug.Log("[EnemySpawner] NavMesh baked (" + label + ") with "
                      + vertices + " vertices.");
        } else {
            Debug.LogWarning("[EnemySpawner] NavMesh bake produced no walkable data (" + label + ").");
        }
    }

    static bool HasNavMeshData() {
        return NavMesh.CalculateTriangulation().vertices.Length > 0;
    }

    /// <summary>
    /// Sample ground-level walkable points from the baked NavMesh using its
    /// triangulation vertices (always valid walkable positions). Points are
    /// spread out over the map so bots cover streets and building interiors.
    /// </summary>
    List<Vector3> SampleWalkablePoints(int count) {
        List<Vector3> points = new List<Vector3>();
        NavMeshTriangulation triangulation = NavMesh.CalculateTriangulation();
        if (triangulation.vertices.Length == 0) {
            return points;
        }

        // Ground level = lowest walkable y; keep only points near it so bots
        // do not spawn on rooftops.
        float groundY = float.MaxValue;
        foreach (Vector3 vertex in triangulation.vertices) {
            groundY = Mathf.Min(groundY, vertex.y);
        }
        List<Vector3> candidates = new List<Vector3>();
        foreach (Vector3 vertex in triangulation.vertices) {
            if (vertex.y - groundY < 2.5f) {
                candidates.Add(vertex);
            }
        }
        if (candidates.Count == 0) {
            candidates.AddRange(triangulation.vertices);
        }

        const float minSpacing = 10.0f;
        const float minSpacingSqr = minSpacing * minSpacing;
        for (int attempt = 0; attempt < 400 && points.Count < count; attempt++) {
            Vector3 candidate = candidates[Random.Range(0, candidates.Count)];
            bool tooClose = false;
            foreach (Vector3 existing in points) {
                if ((existing - candidate).sqrMagnitude < minSpacingSqr) {
                    tooClose = true;
                    break;
                }
            }
            if (!tooClose) {
                points.Add(candidate);
            }
        }
        // Fill any remainder with arbitrary candidates so the quota is met.
        while (points.Count < count && candidates.Count > 0) {
            points.Add(candidates[Random.Range(0, candidates.Count)]);
        }
        return points;
    }

}
