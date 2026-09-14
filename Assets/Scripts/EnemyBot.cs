using Photon.Pun;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

/// <summary>
/// An AI-controlled enemy bot that patrols the city map, investigates the
/// sound of gunfire (via GunshotEvent), chases and attacks players, and can
/// be shot by players.
///
/// Bots are intentionally local (non-networked): every client simulates its
/// own bots so the game works solo against bots. They use the baked NavMesh
/// for navigation (streets + building interiors) and the existing
/// "Shootable" layer / "Player" tag, so no new tags or layers are required.
/// </summary>
[RequireComponent(typeof(NavMeshAgent))]
[RequireComponent(typeof(Rigidbody))]
[RequireComponent(typeof(CapsuleCollider))]
public class EnemyBot : MonoBehaviour {

    public enum BotState { Patrol, Investigate, Chase, Attack, Dead }

    [Header("Perception")]
    [Tooltip("Maximum distance at which the bot can see a player.")]
    [SerializeField]
    private float viewDistance = 45.0f;
    [Tooltip("Horizontal field of view in degrees.")]
    [SerializeField]
    private float fieldOfView = 140.0f;
    [Tooltip("Radius at which the sound of a gunshot is heard (open line of sight).")]
    [SerializeField]
    private float hearingRadius = 55.0f;
    [Tooltip("Seconds between vision checks.")]
    [SerializeField]
    private float visionTick = 0.25f;

    [Header("Combat")]
    [SerializeField]
    private int damagePerShot = 10;
    [Tooltip("Distance at which the bot stops and opens fire.")]
    [SerializeField]
    private float attackRange = 18.0f;
    [SerializeField]
    private float minFireInterval = 0.7f;
    [SerializeField]
    private float maxFireInterval = 1.4f;
    [Tooltip("Random directional jitter per bullet (accuracy).")]
    [SerializeField]
    private float shotSpread = 0.06f;
    [Tooltip("Seconds the bot remembers the last known player position.")]
    [SerializeField]
    private float memoryTime = 6.0f;

    [Header("Health")]
    [SerializeField]
    private int startingHealth = 100;

    /// <summary>Current AI state.</summary>
    public BotState State { get; private set; }

    /// <summary>Human readable name used for kill messages.</summary>
    public string BotName {
        get { return "Bot " + botId; }
    }

    private static int botCounter = 0;
    private int botId;

    private NavMeshAgent agent;
    private CapsuleCollider selfCollider;
    private int health;
    private bool isDead;

    private Transform attackTarget;
    private Vector3 lastKnownPlayerPos;
    private float lostSightTimer;
    private float investigateTimer;
    private Vector3 patrolAnchor;
    private float idleTimer;

    private Transform eye;
    private Transform gunTip;
    private LineRenderer tracer;
    private AudioSource audioSource;
    private AudioClip gunshotClip;
    private float nextFireTime;

    private const float PatrolRadius = 40.0f;
    private const float ProbeRadius = 5.0f;

    // Raycast mask for vision/bullets: ignore cosmetic / view-only layers;
    // everything else can block bullets and line of sight (buildings, doors,
    // players, other bots). Computed in Awake (Unity API is not allowed in
    // static field initializers).
    private static int visionMask = -1;

    void Awake() {
        botId = ++botCounter;
        State = BotState.Patrol;
        health = startingHealth;
        agent = GetComponent<NavMeshAgent>();
        selfCollider = GetComponent<CapsuleCollider>();

        if (visionMask == -1) {
            visionMask = ~(LayerMask.GetMask("Hidden", "Impact", "FPSGun", "TPSGun"));
        }

        Rigidbody body = GetComponent<Rigidbody>();
        body.isKinematic = true;
        body.useGravity = false;

        selfCollider.center = new Vector3(0.0f, 0.95f, 0.0f);
        selfCollider.height = 1.9f;
        selfCollider.radius = 0.4f;

        agent.radius = 0.4f;
        agent.height = 1.9f;
        agent.speed = 3.5f;
        agent.acceleration = 10.0f;
        agent.angularSpeed = 240.0f;
        agent.stoppingDistance = 1.2f;

        BuildRig();
    }

    void Start() {
        patrolAnchor = transform.position;
        idleTimer = Random.Range(1.0f, 4.0f);
        gunshotClip = Resources.Load<AudioClip>("EnemyGunShot");
        StartCoroutine(VisionLoop());
    }

    void OnEnable() {
        GunshotEvent.OnGunshot += OnGunshotHeard;
    }

    void OnDisable() {
        GunshotEvent.OnGunshot -= OnGunshotHeard;
    }

    /// <summary>
    /// Update is called every frame; drives the AI state machine.
    /// </summary>
    void Update() {
        switch (State) {
            case BotState.Patrol:
                UpdatePatrol();
                break;
            case BotState.Investigate:
                UpdateInvestigate();
                break;
            case BotState.Chase:
                UpdateChase();
                break;
            case BotState.Attack:
                UpdateAttack();
                break;
        }
    }

    /// <summary>Wander around the spawn anchor with idle pauses.</summary>
    void UpdatePatrol() {
        if (agent.hasPath && agent.remainingDistance > 1.0f) {
            return;
        }
        idleTimer -= Time.deltaTime;
        if (idleTimer <= 0.0f) {
            Vector3 destination = PickPatrolPoint();
            if (agent.SetDestination(destination)) {
                idleTimer = Random.Range(3.0f, 8.0f);
            } else {
                idleTimer = 1.0f;
            }
        }
    }

    /// <summary>Move to the last heard/seen position and look around.</summary>
    void UpdateInvestigate() {
        investigateTimer -= Time.deltaTime;
        if (!agent.hasPath || agent.remainingDistance < 1.5f) {
            // Arrived: sweep the area with the eyes.
            agent.updateRotation = false;
            transform.Rotate(0.0f, 70.0f * Time.deltaTime, 0.0f);
        }
        if (investigateTimer <= 0.0f) {
            agent.updateRotation = true;
            EnterPatrol();
        }
    }

    /// <summary>Run towards the last known player position.</summary>
    void UpdateChase() {
        lostSightTimer -= Time.deltaTime;
        agent.SetDestination(lastKnownPlayerPos);
        bool arrived = agent.remainingDistance < 2.0f
                       && agent.velocity.sqrMagnitude < 0.5f;
        if (arrived && lostSightTimer <= 0.0f) {
            // Reached the last known spot without spotting anyone: search.
            State = BotState.Investigate;
            investigateTimer = memoryTime;
        }
    }

    /// <summary>Stand still, face the target and fire at it.</summary>
    void UpdateAttack() {
        if (attackTarget == null) {
            ExitAttack();
            State = BotState.Chase;
            lostSightTimer = memoryTime;
            agent.SetDestination(lastKnownPlayerPos);
            return;
        }
        agent.updateRotation = false;
        Vector3 targetPoint = attackTarget.position + Vector3.up * 1.2f;
        Vector3 lookDir = targetPoint - transform.position;
        lookDir.y = 0.0f;
        if (lookDir.sqrMagnitude > 0.01f) {
            transform.rotation = Quaternion.RotateTowards(
                transform.rotation,
                Quaternion.LookRotation(lookDir.normalized),
                agent.angularSpeed * Time.deltaTime);
        }
        if (Time.time >= nextFireTime && HasLineOfSight(targetPoint)) {
            Fire();
        }
    }

    void EnterPatrol() {
        State = BotState.Patrol;
        idleTimer = Random.Range(1.0f, 3.0f);
        agent.ResetPath();
    }

    void EnterAttack(Transform target) {
        attackTarget = target;
        State = BotState.Attack;
        agent.isStopped = true;
        agent.ResetPath();
        nextFireTime = Time.time + Random.Range(0.2f, 0.6f);
    }

    void ExitAttack() {
        attackTarget = null;
        agent.isStopped = false;
        agent.updateRotation = true;
    }

    /// <summary>
    /// Periodically scan for players and update the AI state based on what
    /// the bot can see.
    /// </summary>
    IEnumerator VisionLoop() {
        WaitForSeconds wait = new WaitForSeconds(visionTick);
        while (!isDead) {
            EvaluateVision();
            yield return wait;
        }
    }

    void EvaluateVision() {
        GameObject[] players = GameObject.FindGameObjectsWithTag("Player");
        GameObject best = null;
        float bestDistance = float.MaxValue;
        foreach (GameObject candidate in players) {
            Vector3 targetPoint = candidate.transform.position + Vector3.up * 1.2f;
            float distance = Vector3.Distance(eye.position, targetPoint);
            if (distance > viewDistance || distance >= bestDistance) {
                continue;
            }
            Vector3 direction = (targetPoint - eye.position).normalized;
            if (Vector3.Angle(transform.forward, direction) > fieldOfView * 0.5f) {
                continue;
            }
            if (!HasLineOfSight(targetPoint)) {
                continue;
            }
            best = candidate;
            bestDistance = distance;
        }

        if (best != null) {
            lastKnownPlayerPos = best.transform.position;
            lostSightTimer = memoryTime;
            if (State == BotState.Attack) {
                attackTarget = best.transform;
                if (bestDistance > attackRange * 1.3f) {
                    // Target ran out of effective range: chase it instead.
                    ExitAttack();
                    State = BotState.Chase;
                    agent.SetDestination(lastKnownPlayerPos);
                }
            } else if (bestDistance <= attackRange) {
                EnterAttack(best.transform);
            } else {
                State = BotState.Chase;
                agent.SetDestination(lastKnownPlayerPos);
            }
        } else if (State == BotState.Attack) {
            // Lost sight of the target while attacking.
            ExitAttack();
            State = BotState.Chase;
            lostSightTimer = memoryTime;
            agent.SetDestination(lastKnownPlayerPos);
        }
    }

    /// <summary>
    /// Reaction to the sound of a gunshot broadcast on GunshotEvent. Bots
    /// within earshot rush to investigate where the shot came from. Walls
    /// muffle the sound: half the hearing radius applies when there is no
    /// line of sight to the shooter.
    /// </summary>
    void OnGunshotHeard(Vector3 position, object source) {
        if (isDead || ReferenceEquals(source, this)) {
            return;
        }
        float distance = Vector3.Distance(transform.position, position);
        if (distance > hearingRadius) {
            return;
        }
        if (distance > hearingRadius * 0.5f && !HasLineOfSight(position)) {
            return;
        }
        lastKnownPlayerPos = position;
        if (State == BotState.Patrol || State == BotState.Investigate
            || State == BotState.Chase) {
            State = BotState.Investigate;
            investigateTimer = memoryTime;
            agent.updateRotation = true;
            agent.SetDestination(lastKnownPlayerPos);
        }
    }

    /// <summary>
    /// True when nothing solid blocks the straight line between the bot's
    /// eyes and the given point.
    /// </summary>
    bool HasLineOfSight(Vector3 point) {
        if (eye == null) {
            return false;
        }
        Vector3 origin = eye.position;
        Vector3 direction = point - origin;
        float distance = direction.magnitude;
        if (distance < 0.001f) {
            return true;
        }
        direction /= distance;
        RaycastHit[] hits = Physics.RaycastAll(origin, direction, distance, visionMask);
        foreach (RaycastHit hit in hits) {
            if (hit.collider.transform.IsChildOf(transform)) {
                continue;
            }
            if (hit.distance < distance - 0.4f) {
                return false;
            }
        }
        return true;
    }

    /// <summary>
    /// Fire one hitscan shot at the current attack target. Damages real
    /// players through their existing TakeDamage Photon RPC and broadcasts
    /// the gunshot so other bots can hear it.
    /// </summary>
    void Fire() {
        nextFireTime = Time.time + Random.Range(minFireInterval, maxFireInterval);
        Vector3 targetPoint = attackTarget.position + Vector3.up * 1.2f;
        Vector3 direction = (targetPoint - eye.position).normalized;
        direction += Random.insideUnitSphere * shotSpread;
        direction.Normalize();

        Vector3 endPoint = eye.position + direction * viewDistance;
        RaycastHit[] hits = Physics.RaycastAll(eye.position, direction, viewDistance, visionMask);
        System.Array.Sort(hits, (a, b) => a.distance.CompareTo(b.distance));
        foreach (RaycastHit hit in hits) {
            if (hit.collider.transform.IsChildOf(transform)) {
                continue;
            }
            endPoint = hit.point;
            EnemyBot hitBot = hit.collider.GetComponentInParent<EnemyBot>();
            if (hitBot == null && hit.collider.gameObject.tag == "Player") {
                PhotonView photonView = hit.collider.GetComponentInParent<PhotonView>();
                if (photonView != null) {
                    photonView.RPC("TakeDamage", RpcTarget.All, damagePerShot, BotName);
                    Object fleshImpact = Resources.Load("impactFlesh");
                    if (fleshImpact != null) {
                        Instantiate(
                            fleshImpact,
                            hit.point,
                            Quaternion.Euler(hit.normal.x - 90, hit.normal.y, hit.normal.z));
                    }
                }
            }
            break; // First solid hit stops the bullet (bots ignore each other).
        }

        DrawTracer(gunTip.position, endPoint);
        if (audioSource != null && gunshotClip != null) {
            audioSource.PlayOneShot(gunshotClip);
        }
        GunshotEvent.Broadcast(eye.position, this);
    }

    /// <summary>Flash the tracer line for a few frames.</summary>
    void DrawTracer(Vector3 from, Vector3 to) {
        if (tracer == null) {
            return;
        }
        tracer.SetPosition(0, from);
        tracer.SetPosition(1, to);
        tracer.enabled = true;
        StartCoroutine(HideTracer());
    }

    IEnumerator HideTracer() {
        yield return new WaitForSeconds(0.06f);
        if (tracer != null) {
            tracer.enabled = false;
        }
    }

    /// <summary>
    /// Called by player weapons when a bullet hits this bot. The bot also
    /// becomes alert and investigates where the shot came from.
    /// </summary>
    public void TakeDamage(int amount, Vector3 shooterPosition) {
        if (isDead) {
            return;
        }
        health -= amount;
        if (health <= 0) {
            Die();
            return;
        }
        // Getting shot reveals the shooter's general position.
        lastKnownPlayerPos = shooterPosition;
        if (State == BotState.Patrol || State == BotState.Investigate) {
            State = BotState.Investigate;
            investigateTimer = memoryTime;
            agent.SetDestination(lastKnownPlayerPos);
        }
    }

    void Die() {
        isDead = true;
        State = BotState.Dead;
        if (tracer != null) {
            tracer.enabled = false;
        }
        StopAllCoroutines();
        agent.isStopped = true;
        agent.enabled = false;
        selfCollider.enabled = false;
        GunshotEvent.OnGunshot -= OnGunshotHeard;
        if (EnemySpawner.Instance != null) {
            EnemySpawner.Instance.NotifyBotDied(this);
        }
        StartCoroutine(DeathAnimation());
    }

    /// <summary>Tip over, sink into the ground and clean up.</summary>
    IEnumerator DeathAnimation() {
        float tipTime = 0.0f;
        while (tipTime < 1.0f) {
            tipTime += Time.deltaTime * 2.0f;
            transform.rotation = Quaternion.Lerp(
                transform.rotation,
                Quaternion.Euler(90.0f, transform.eulerAngles.y, 0.0f),
                Time.deltaTime * 4.0f);
            yield return null;
        }
        yield return new WaitForSeconds(1.5f);
        float sinkTime = 0.0f;
        while (sinkTime < 2.0f) {
            sinkTime += Time.deltaTime;
            transform.position += Vector3.down * 0.4f * Time.deltaTime;
            yield return null;
        }
        Destroy(gameObject);
    }

    /// <summary>Pick a random walkable point to patrol to near the anchor.</summary>
    Vector3 PickPatrolPoint() {
        List<Vector3> candidates = EnemySpawner.WalkablePoints;
        for (int attempt = 0; attempt < 10; attempt++) {
            Vector3 candidate;
            if (candidates.Count > 0) {
                candidate = candidates[Random.Range(0, candidates.Count)];
                // Prefer points within patrol range of the anchor.
                if ((candidate - patrolAnchor).sqrMagnitude
                    > PatrolRadius * PatrolRadius) {
                    continue;
                }
            } else {
                Vector2 offset = Random.insideUnitCircle * PatrolRadius;
                candidate = patrolAnchor + new Vector3(offset.x, 0.0f, offset.y);
            }
            NavMeshHit hit;
            if (NavMesh.SamplePosition(candidate, out hit, ProbeRadius, NavMesh.AllAreas)) {
                return hit.position;
            }
        }
        return patrolAnchor;
    }

    /// <summary>
    /// Build a simple robotic soldier visual: capsule body, head, gun and a
    /// tracer line. Everything is created at runtime so no prefab assets are
    /// required and the bot works on the existing "Shootable" layer.
    /// </summary>
    void BuildRig() {
        Shader shader = Shader.Find("Standard");
        if (shader == null) {
            shader = Shader.Find("Legacy Shaders/Diffuse");
        }
        if (shader == null) {
            shader = Shader.Find("Sprites/Default");
        }

        // Body.
        GameObject bodyObject = GameObject.CreatePrimitive(PrimitiveType.Capsule);
        DestroyImmediate(bodyObject.GetComponent<Collider>());
        bodyObject.transform.SetParent(transform, false);
        bodyObject.transform.localPosition = new Vector3(0.0f, 0.9f, 0.0f);
        bodyObject.transform.localScale = new Vector3(0.7f, 1.0f, 0.7f);
        bodyObject.GetComponent<Renderer>().sharedMaterial =
            MakeMaterial(shader, new Color(0.55f, 0.12f, 0.12f));

        // Head.
        GameObject headObject = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        DestroyImmediate(headObject.GetComponent<Collider>());
        headObject.transform.SetParent(transform, false);
        headObject.transform.localPosition = new Vector3(0.0f, 1.75f, 0.0f);
        headObject.transform.localScale = new Vector3(0.5f, 0.5f, 0.5f);
        headObject.GetComponent<Renderer>().sharedMaterial =
            MakeMaterial(shader, new Color(0.25f, 0.25f, 0.28f));

        // Gun.
        GameObject gunObject = GameObject.CreatePrimitive(PrimitiveType.Cube);
        DestroyImmediate(gunObject.GetComponent<Collider>());
        gunObject.transform.SetParent(transform, false);
        gunObject.transform.localPosition = new Vector3(0.28f, 1.25f, 0.45f);
        gunObject.transform.localScale = new Vector3(0.12f, 0.14f, 0.55f);
        gunObject.GetComponent<Renderer>().sharedMaterial =
            MakeMaterial(shader, new Color(0.1f, 0.1f, 0.1f));

        // Muzzle tip for the tracer.
        GameObject tipObject = new GameObject("GunTip");
        tipObject.transform.SetParent(gunObject.transform, false);
        tipObject.transform.localPosition = new Vector3(0.0f, 0.0f, 0.4f);
        gunTip = tipObject.transform;

        // Eyes (slightly in front of the head).
        GameObject eyeObject = new GameObject("Eye");
        eyeObject.transform.SetParent(headObject.transform, false);
        eyeObject.transform.localPosition = new Vector3(0.0f, 0.0f, 0.28f);
        eye = eyeObject.transform;

        // Tracer line.
        GameObject tracerObject = new GameObject("Tracer");
        tracerObject.transform.SetParent(transform, false);
        tracer = tracerObject.AddComponent<LineRenderer>();
        tracer.positionCount = 2;
        tracer.widthMultiplier = 0.04f;
        tracer.useWorldSpace = true;
        tracer.enabled = false;
        Shader lineShader = Shader.Find("Sprites/Default");
        if (lineShader != null) {
            tracer.material = new Material(lineShader);
        }

        // Gunshot audio (3D so the player can hear where bots are shooting).
        audioSource = gameObject.AddComponent<AudioSource>();
        audioSource.spatialBlend = 1.0f;
        audioSource.maxDistance = 80.0f;
        audioSource.playOnAwake = false;

        // Whole bot lives on the Shootable layer so player bullets hit it.
        SetLayerRecursive(gameObject, LayerMask.NameToLayer("Shootable"));
    }

    static Material MakeMaterial(Shader shader, Color color) {
        Material material = new Material(shader);
        if (material.HasProperty("_Color")) {
            material.color = color;
        }
        return material;
    }

    static void SetLayerRecursive(GameObject root, int layer) {
        root.layer = layer;
        foreach (Transform child in root.transform) {
            SetLayerRecursive(child.gameObject, layer);
        }
    }

}
