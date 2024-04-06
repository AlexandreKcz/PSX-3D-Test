#include "constants.h"
#include "controller.h"
#include "readCD.C"
#include "2D.c"
#include "3D.c"
#include "audio.c"

void Initialize();
void Start();
void Update();
void Render();
void Controls();

int movementSpeed = 7;
int rotationSpeed = 30;

u_long* cdData[3];

struct {
	VECTOR position;
	SVECTOR rotation;
} car;

struct {
	VECTOR position;
	SVECTOR rotation;
} grid;

int main() {
	Initialize();
	Start();
	while(1) {
		Update();
		Render();
	}

	return 0;
}

void Initialize(){
    ReadCDInit();
    initializeScreen();
    initializePad();
    cd_open();
    cd_read_file("WRENCH.TMD", &cdData[0]);
    cd_read_file("CAR.TIM", &cdData[1]);
    cd_read_file("GRID.TMD", &cdData[2]);

    cd_close();

    loadTexture((u_char *)cdData[1]);
}

void Start() {
    ObjectCount += LoadTMD(cdData[2], &Object[0], 1); /* Grid */
	ObjectCount += LoadTMD(cdData[0], &Object[1], 1); /* Cube */

    //Camera
	Camera.position.vx = -5000;
	Camera.position.vy = 2500;
	Camera.position.vz = 5000;

	//Camera.rotation.vx = 400;
	//Camera.rotation.vy = 400;

	//Car
	car.position.vx = 3500;
	car.position.vy = 924;
	car.position.vz = -3500;

	car.rotation.vy = 900;

    //The background color of the scene
	//in RGB (values between 0-255)
	setBGColor(157, 176, 209);

	//Set the color of ambient light in RGB
	SetAmbientLight(67, 67, 67);

	//The sunlight color in RGB
	SetSunColor(255, 255, 255);

	//Sunlight direction
	SetSunDirection(0, -1, 1);
}

void Update () {
    //this gets the status of all the controller buttons
    //like if they are pressed or not.
	padUpdate();

	//do stuff when buttons are pressed (more info below)
	Controls();
}

void Render () {

	// Prepare for rendering
	clear_display();

	// Render debug text. this is good for seeing what
	//some variables are doing while the game is running
	//or just to display some text to the player.

	/*
	FntPrint("X: forward\n");
	FntPrint("Square: reverse\n");
	FntPrint("D-pad: Steer\n");
	FntPrint("Select: Reset RC Car\n");
	*/

	FntPrint("Camera Rotation : (%d, %d, %d)\n", Camera.rotation.vx, Camera.rotation.vy, Camera.rotation.vz);
	FntPrint("Camera Position : (%d, %d, %d)\n", Camera.position.vx, Camera.position.vy, Camera.position.vz);

	//FntPrint("Car Position: (%d, %d, %d)\n", car.position.vx, car.position.vy, car.position.vz);
	//FntPrint("Car Rotation: (%d, %d, %d)\n", car.rotation.vx, car.rotation.vy, car.rotation.vz);

	// Calculate the camera and viewpoint
	CalculateCamera();

	// Sort and render objects
	RenderObject(grid.position, grid.rotation, &Object[0]);
	RenderObject(car.position, car.rotation, &Object[1]);

	//Showtime!
	Display();

}

void Controls () {

    //used later to store a vector for local forward movement
    VECTOR newMoveVec;

	if (padCheck(Pad1Left)) {
		//Turn car Left
		//car.rotation.vy -= rotationSpeed;
		Camera.rotation.vy += rotationSpeed;
	}

	if (padCheck(Pad1Right)) {
		//Turn car Right
		//car.rotation.vy += rotationSpeed;
		Camera.rotation.vy -= rotationSpeed;
	}

	if (padCheck(Pad1Up)) {

		SVECTOR rerootedForward = Camera.rotation;
		rerootedForward.vx *= -1;
		rerootedForward.vy *= -1;
		rerootedForward.vz *= -1;

		newMoveVec = Translate(rerootedForward, 0, 0, -movementSpeed * ONE/500);
		
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Down)) {

		SVECTOR rerootedForward = Camera.rotation;
		rerootedForward.vx *= -1;
		rerootedForward.vy *= -1;
		rerootedForward.vz *= -1;

		newMoveVec = Translate(rerootedForward, 0, 0, movementSpeed * ONE/500);
		
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Cross)) {
		//Move car forwards
		/*newMoveVec = Translate(car.rotation, 0, 0, movementSpeed * ONE/500);
		car.position.vx += newMoveVec.vx;
		car.position.vy += newMoveVec.vy;
		car.position.vz += newMoveVec.vz;*/

		newMoveVec = Translate(Camera.rotation, 0, movementSpeed * ONE/500, 0);
		
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Square)) {
		//Move car backwards
		/*newMoveVec = Translate(car.rotation, 0, 0, -movementSpeed * ONE/500);
		car.position.vx += newMoveVec.vx;
		car.position.vy += newMoveVec.vy;
		car.position.vz += newMoveVec.vz;*/

		newMoveVec = Translate(Camera.rotation, 0, -movementSpeed * ONE/500, 0);
		
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Select)) {
		//If you drive too far off screen and get lost, you may
		//want to reset your car where you can see it again.
		car.position.vx = 0;
		car.position.vz = 0;
	}

}