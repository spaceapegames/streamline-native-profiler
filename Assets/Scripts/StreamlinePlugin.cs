using UnityEngine;
using System.Runtime.InteropServices;

public class StreamlinePlugin : MonoBehaviour
{
#if UNITY_ANDROID && !UNITY_EDITOR
    [DllImport("streamline_unity")]
    private static extern void InitStreamlineUnityPlugin();

    [DllImport("streamline_unity")]
    private static extern void MarkStreamlineRegion(string name);

    void Start ()
    {
        InitStreamlineUnityPlugin();
    }
#endif

    public static void BeginRegion(string name)
    {
#if UNITY_ANDROID && !UNITY_EDITOR
        MarkStreamlineRegion("Region Start " + name);
#endif
    }

    public static void EndRegion(string name)
    {
#if UNITY_ANDROID && !UNITY_EDITOR
        MarkStreamlineRegion("Region End " + name);
#endif
    }
}