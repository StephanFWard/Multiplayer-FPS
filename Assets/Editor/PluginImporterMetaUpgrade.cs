#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;

internal static class PluginImporterMetaUpgrade
{
    public static void ResaveLegacyPhotonImporters()
    {
        Resave("Assets/Photon/PhotonLibs/Metro/Photon3Unity3D.dll");
        Resave("Assets/Photon/PhotonLibs/WebSocket/WebSocket.jslib");
        AssetDatabase.SaveAssets();
    }

    private static void Resave(string assetPath)
    {
        PluginImporter importer = AssetImporter.GetAtPath(assetPath) as PluginImporter;
        if (importer == null)
        {
            Debug.LogError("PluginImporter not found: " + assetPath);
            return;
        }

        EditorUtility.SetDirty(importer);
        importer.SaveAndReimport();
    }
}
#endif
