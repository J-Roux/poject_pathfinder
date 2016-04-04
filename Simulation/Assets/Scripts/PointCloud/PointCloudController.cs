using UnityEngine;
using System.Collections;

public class PointCloudController : MonoBehaviour {

    public float MoveSpeed = 3f;

    private float rotationY = 0f;
    private float minY = -89f;
    private float maxY = 89f;

    private void TranslateCamera(Vector3 where) {
        Camera.main.transform.Translate(where * (Time.deltaTime * MoveSpeed), Space.Self);
    }

    private void HandleKeyboardControls()
    {
        if (Input.GetKey("w"))
            TranslateCamera(Vector3.forward);
        else if (Input.GetKey("s"))
            TranslateCamera(-Vector3.forward);

        if (Input.GetKey("a"))
            TranslateCamera(Vector2.left);
        else if (Input.GetKey("d"))
            TranslateCamera(-Vector3.left);

        if (Input.GetKey(KeyCode.Space))
            TranslateCamera(Vector3.up);
        else if (Input.GetKey(KeyCode.LeftShift))
            TranslateCamera(-Vector3.up);
    }

    private void HandleMouseControls()
    {
        float rotationX = Camera.main.transform.localEulerAngles.y + Input.GetAxis("Mouse X");
        
        rotationY += Input.GetAxis("Mouse Y");
        rotationY = Mathf.Clamp (rotationY, minY, maxY);
        
        Camera.main.transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
    }

    private void Update() {
        HandleKeyboardControls();
        HandleMouseControls();
    }
}
