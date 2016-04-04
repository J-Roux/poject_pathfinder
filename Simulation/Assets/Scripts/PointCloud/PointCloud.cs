using UnityEngine;
using System.Collections;


[RequireComponent(typeof(MeshFilter), typeof(MeshRenderer))]
public class PointCloud : MonoBehaviour {
    
    private Mesh mesh;
    int numPoints = 60000;

    private void Start () {
        mesh = new Mesh();
        GetComponent<MeshFilter>().mesh = mesh;

        CreateMesh();
    }

    float NormalOf(float y) {
        //return Mathf.Exp(-Mathf.Pow(2.5f * x, 2) / 2);
        return Mathf.Sqrt(-2f * Mathf.Log(y)) / 2.5f;
    }

    void CreateMesh() {

        Vector3[] points = new Vector3[numPoints];
        int[] indecies = new int[numPoints];
        Color[] colors = new Color[numPoints];

        for(int i=0; i < points.Length; i++) {
            var x = 10f * NormalOf(Random.Range(0f, 1f));
            var y = 10f * NormalOf(Random.Range(0f, 1f));
			var z = 10f * NormalOf(Random.Range(0f, 1f));

            points[i] = new Vector3(x, y, z);

            indecies[i] = i;

            colors[i] = new Color(x / 10f, NormalOf(Random.Range(0f, 1f)), y / 10f, 1.0f);
        }
        
        mesh.vertices = points;
        mesh.colors = colors;
        mesh.SetIndices(indecies, MeshTopology.Points, 0);
    }
}