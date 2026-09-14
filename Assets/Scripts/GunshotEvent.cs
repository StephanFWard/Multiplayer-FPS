using UnityEngine;

/// <summary>
/// Global event bus for gunshots.
///
/// Any weapon (player guns, enemy bots) broadcasts the world position of each
/// shot it fires. AI agents (EnemyBot) subscribe and use the events to become
/// "spatially aware" of the sound of gunfire: they react to shots fired within
/// earshot, using distance and wall occlusion to decide whether they heard it.
/// </summary>
public static class GunshotEvent {

    /// <summary>
    /// Listener signature. <paramref name="source"/> lets listeners ignore
    /// their own gunshots.
    /// </summary>
    public delegate void GunshotHandler(Vector3 position, object source);

    /// <summary>Raised every time any weapon fires a shot.</summary>
    public static event GunshotHandler OnGunshot;

    /// <summary>
    /// Broadcast a single gunshot originating at <paramref name="position"/>.
    /// </summary>
    /// <param name="position">World position the shot was fired from.</param>
    /// <param name="source">The shooter (or its weapon); used to filter out self-hearing.</param>
    public static void Broadcast(Vector3 position, object source) {
        if (OnGunshot != null) {
            OnGunshot(position, source);
        }
    }

}
