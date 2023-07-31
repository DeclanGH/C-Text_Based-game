/* Declan ONUNKWO		*
 * CSC322 - Systems Programming *
 * Assignment 1 - Game in C 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

	int respect = 40; // Initializing a global respect value

	typedef struct Room_struct Room;
	typedef struct Creature_struct Creature;
 
struct Room_struct { // Struct to hold Room information
	int rmState; // 0 for clean, 1 for half-dirty, 2 for dirty
	int rmNum;
	int creatureSize; 
	Room* rmNorth; // line {17-20} holds the number cardinal directions (-1 if no neighbour)
	Room* rmSouth;
	Room* rmEast;
	Room* rmWest;
	Creature* creatures[10]; // Max of 10 per room   
};

struct Creature_struct { // Struct to hold Creature information
	int creatureID; // Holds the number/ID of the creature
	int creatureType; // 0 for PC, 1 for animal, 2 for Human
	Room* creatureRoom; // Holds the information of the creatures current room 	
};

	// Declaring global variables
	Room* pRoom = NULL;
	Creature* pCreature = NULL;
	Creature* pPC = NULL;

	// Prototytping methods
	void creatingWorld();
	void commands();
	void look();
	int notFull(Room* pRm);
	int movePC(Creature* creature, Room* coordinate);
	void oldRoomUpdate(Creature* cre1, Room* rm1);
	void newRoomUpdate(Creature* cre, Room* rm);
	void clean(Creature* cre2);
	void dirty(Creature* cre3);
	void reaction(Creature* cre4, int state);
	void moveNPC(Creature* cre5);
	void updateOldRoomNPC(Room* rm);
	void leaveWorld(Creature* crea);	
	void creatureClean(Creature* cre6);

int main()
{
        creatingWorld();
 
        free(pRoom);
        free(pCreature);
	printf("\nGOODBYE!\n"); // if you see this message, then memory was freed.
 
        return 0;
}
	
	// Prompting the user and World Creation
	void creatingWorld(){
	int totalRooms, state, north, south, east, west; // variables for scanner
	
	printf("\nEnter the number of rooms you want in your world: ");
	scanf("%d", &totalRooms);

	if (totalRooms <=  1 || totalRooms >= 100){ // Condition for 1 < totalRooms < 100
		printf("You are limited to 2 to 99 rooms in your world!\n");
		creatingWorld();	
	}else{ 
		pRoom = (Room*) malloc(totalRooms * sizeof(Room));
	}

	printf("\nRoom State: 0 = clean, 1 = half dirty, 1 = dirty\n"); 
        printf("Neighbor: North, South, East and West respectively\n");
	printf("Use the Room Number of a room to indicate its bearings to the current room and -1 for no neighbor.\n\n");

	printf("Use integers to indicate State, NorthRoom, SouthRoom, EastRoom and WestRoom\n");
	for (int i=0; i<totalRooms; i++){
		pRoom[i].creatureSize = 0;
		pRoom[i].rmNum = i;
		
		printf("Room %d: ", i);
		scanf("%d %d %d %d %d", &state, &north, &south, &east, &west);
		pRoom[i].rmState = state;
	
		// Assigning neighbors and checking errors
		if (north == -1){
			pRoom[i].rmNorth = NULL;
		}else{
			pRoom[i].rmNorth = &pRoom[north];	
		}

		if (south == -1){
			pRoom[i].rmSouth = NULL;
		}else{
			pRoom[i].rmSouth = &pRoom[south];	
		}

		if (east == -1){
			pRoom[i].rmEast = NULL;
		}else{
			pRoom[i].rmEast = &pRoom[east];	
		}

		if (west == -1){
			pRoom[i].rmWest = NULL;
		}else{
			pRoom[i].rmWest = &pRoom[west];	
		} 
	}

	int numOfCreatures, typeOfCreature, creatureSpawnRoom; // Scanner variables for creature

	printf("\nInput the number of creatures you want in your world: ");
	scanf("%d", &numOfCreatures);
	pCreature = (Creature*) malloc(numOfCreatures * sizeof(Creature));
	
	printf("\nCreatures: 0 for PC, 1 for Animal, 2 for Human.\n");
	printf("For each creature, indicate their ID and spawn room respectively using the provided integer.\n");

	for (int j=0; j<numOfCreatures; j++){
		printf("Creature %d: ", j);
		scanf("%d %d", &typeOfCreature, &creatureSpawnRoom);
		pCreature[j].creatureID = j;
		pCreature[j].creatureType = typeOfCreature;
		pCreature[j].creatureRoom = &pRoom[creatureSpawnRoom];
	
		if (pCreature[j].creatureType == 0){ // Condition to ensure PC is assigned to '0' 
			pPC = &pCreature[j];
		}
	}	

	// Building the world
	int x = 0; // iterator for rooms
	while (x < totalRooms){
		int y = 0; // iterator for creatures 
		while (y < numOfCreatures){
			if (pCreature[y].creatureRoom == &pRoom[x]){
				pRoom[x].creatures[pRoom[x].creatureSize] = &pCreature[y];
				pRoom[x].creatureSize++;
			}
			y++;
		}
		x++;
	}	

	printf("\nGreat! Your world has been created\n\n");
	printf("COMMANDS: ");
	printf("Type 'look' to get information on the PCs current room.\n");
	printf("Type 'clean' to make the PC clean their current room.\n");
	printf("Type '<ID>:clean' to make the creature with that ID clean the current room.\n");
	printf("Type 'dirty' to make the PC dirty their current room.\n");
	printf("Type '<ID>:dirty' to make the creature with that ID dirty the current room.\n");
	printf("Type 'north' to move the PC to the northern room from their current room\n");
	printf("Type 'south' to move the PC to the southern room from their current room\n");
	printf("Type 'east' to move the PC to the eastern room from their current room\n");
	printf("Type 'west' to move the PC to the western room from their current room\n");
	printf("Type 'exit' to exit the game.\n");
	commands(); // command the user selects	
}

void commands(){
	char userInput[20];

	while(1){

		if (respect <= 0){ // other conditions that may lead to game finish
			printf("Animals and Humans no longer respect you!\nThey unanimously voted to kick you out of your world.");
			break;
        	}else if (respect > 80){
			printf("You are now highly respected! The other creatures praise your hardwork.");
			break;
		}

		printf("\nType a command: ");
		scanf("%s", userInput);		

		if (strcmp(userInput,"look") == 0){
			look();
		}else if (strcmp(userInput,"north") == 0){
			if(movePC(pPC, pPC->creatureRoom->rmNorth)){
				printf("You leave towards the east.\n");
			}else{
				printf("Room is either full or not in this world.\n");
			}
		}else if (strcmp(userInput,"south") == 0){
			if(movePC(pPC, pPC->creatureRoom->rmSouth)){
				printf("You leave towards the south.\n");
			}else{
				printf("Room is either full or not in this world.\n");
			}
		}else if (strcmp(userInput,"east") == 0){
			if(movePC(pPC, pPC->creatureRoom->rmEast)){
				printf("You leave towards the east.\n");
			}else{
				printf("Room is either full or not in this world.\n");
			}
		}else if (strcmp(userInput,"west") == 0){
			if(movePC(pPC, pPC->creatureRoom->rmWest)){
				printf("You leave towards the west.\n");
			}else{
				printf("Room is either full or not in this world.\n");
			}
		}else if (strcmp(userInput,"clean") == 0){
			clean(pPC);
		}else if (strcmp(userInput,"dirty") == 0){
			dirty(pPC);
		}else if (strcmp(userInput,"exit") == 0){
			break;
		}else{
			printf("Invalid command! Try again.\n");
		}
	}
}

void look(){
	char roomStateStr[12];
	int roomStateInt = pPC->creatureRoom->rmState;

	if (roomStateInt == 0){ // Condition to convert integer state to string state
		strcpy(roomStateStr, "clean");
	}else if (roomStateInt == 1){
		strcpy(roomStateStr, "half-dirty");
	}else{
		strcpy(roomStateStr, "dirty");
	}

	printf("Room %d, %s. Contains: PC",pPC->creatureRoom->rmNum, roomStateStr);
	
	for (int i=0; i < pPC->creatureRoom->creatureSize; i++){ // loop through creatures[i]

        int checkCreatureType = pPC->creatureRoom->creatures[i]->creatureType;

        	if (checkCreatureType == 1){
            		printf(", animal %d", pPC->creatureRoom->creatures[i]->creatureID);
        	}else if (checkCreatureType == 2){
            		printf(", human %d", pPC->creatureRoom->creatures[i]->creatureID);
       	 	}
	}

	printf("\nNeighbor(s):-\n");

	if (pPC->creatureRoom->rmNorth != NULL){
		printf("Room %d to the North\n", pPC->creatureRoom->rmNorth->rmNum);
	}
	if (pPC->creatureRoom->rmSouth != NULL){
		printf("Room %d to the South\n", pPC->creatureRoom->rmSouth->rmNum);
	}
	if (pPC->creatureRoom->rmEast != NULL){
		printf("Room %d to the East\n", pPC->creatureRoom->rmEast->rmNum);
	}
	if (pPC->creatureRoom->rmWest != NULL){
		printf("Room %d to the West\n", pPC->creatureRoom->rmWest->rmNum);
	}
}

int notFull(Room* pRm){
	if (pRm->creatureSize == 10) {
        	return 0; // false
	}
	return 1; // true then carry on
}

int movePC(Creature* creature, Room* coordinate){
	if (!notFull(coordinate) || coordinate == NULL){
		return 0; // false. Creature could not move
	}
	
	oldRoomUpdate(creature, creature->creatureRoom);
	newRoomUpdate(creature, coordinate);
	creatureClean(creature);
	updateOldRoomNPC(creature->creatureRoom);

	return 1; // true
}

void oldRoomUpdate(Creature* cre1, Room* rm1){
	for (int i=0; i < rm1->creatureSize; i++){
		if (cre1->creatureID == rm1->creatures[i]->creatureID){
			rm1->creatures[i] = NULL;
			cre1->creatureRoom = NULL;
			rm1->creatureSize = rm1->creatureSize - 1;
			break; // exit loop if creature is found
		}
	}
}

void newRoomUpdate(Creature* cre, Room* rm){
	cre->creatureRoom = rm;		
	rm->creatures[rm->creatureSize] = cre;
	rm->creatureSize = rm->creatureSize + 1;
}

void updateOldRoomNPC(Room* rm){ // when a creature moves update previous room
	int i = 0;
	for (int j=0; j < rm->creatureSize; j++){
		if (rm->creatures[j] !=NULL){
			rm->creatures[i] = rm->creatures[j];
			i++; 
		}
	}
	rm->creatureSize = i;
}

void clean(Creature* cre2){
	int currentState = cre2->creatureRoom->rmState;
	if (currentState == 0){
		printf("No action taken! Room is already clean.\n");
	}else{
		currentState = currentState - 1; 
		cre2->creatureRoom->rmState = currentState;
		reaction(cre2, 0);				
	}
	updateOldRoomNPC(cre2->creatureRoom);
}

void dirty(Creature* cre3){
	int currentState = cre3->creatureRoom->rmState;
   	if(currentState == 2) {
        	printf("No action taken! Room is already dirty.\n");   
    	}else{
        	currentState = currentState + 1;										
        	cre3->creatureRoom->rmState = currentState;								
        	reaction(cre3, 1);
    	}
    	updateOldRoomNPC(cre3->creatureRoom);
}

void reaction(Creature* cre4, int state){
	if (cre4->creatureType == 1){
		if(cre4->creatureRoom->rmState == 2){
			moveNPC(cre4);
			updateOldRoomNPC(pPC->creatureRoom);
		}
	}else if (cre4->creatureType == 2){
		if (cre4->creatureRoom->rmState == 0){
			moveNPC(cre4);
			updateOldRoomNPC(pPC->creatureRoom);
		}
	}

	for (int i=0; i < pPC->creatureRoom->creatureSize; i++){ // loop through every creature in PC room
		Creature* crs = pPC->creatureRoom->creatures[i];

		if (state == 0){
			if (crs->creatureType == 1 && crs->creatureID != cre4->creatureID){
				respect++;
				printf("%d lick. Respect is now %d\n", crs->creatureID, respect);
			}else if(crs->creatureType == 2 && crs->creatureID != cre4->creatureID){
				respect--;
				printf("%d grumbles. Respect is now %d\n", crs->creatureID, respect);
				if (crs->creatureRoom->rmState == 0){
					moveNPC(crs);
				}
			}
		}else if (state == 1){
			if (crs->creatureType == 2 && crs->creatureID != cre4->creatureID){
				respect++;
				printf("%d smiles. Respect is now %d\n",crs->creatureID, respect);
			}else if (crs->creatureType == 1 && crs->creatureID != cre4->creatureID){
				respect--;
				printf("%d growls. Respect is now %d\n",crs->creatureID,respect);
				if (crs->creatureRoom->rmState == 2){
					moveNPC(crs);
				}
			}
		}
	}
}

void moveNPC(Creature* cre){ // NPC random movement when in an uncomfortable room
	srand(time(NULL));
        int randDir = rand()%4; // Random max 4 to output (0-3), only possible remainders
	Room* direction[4] = {cre->creatureRoom->rmNorth, cre->creatureRoom->rmSouth, cre->creatureRoom->rmEast, cre->creatureRoom->rmWest};

	for (int i = 0; i < 4; i++){
		int next = (randDir + i) % 4;
		if (movePC(cre, direction[next])){
			switch (next) {
				case 0:
					printf("%d leaves towards the north\n", cre->creatureID);
                    			break;
                		case 1:
                    			printf("%d leaves towards the south\n", cre->creatureID);
                    			break;
                		case 2:
                    			printf("%d leaves towards the east\n", cre->creatureID);
                    			break;
                		case 3:
                    			printf("%d leaves towards the  west\n", cre->creatureID);
                    			break;
            		}
            		return;
        	}
    	}
    	leaveWorld(cre); 
}

void leaveWorld(Creature* crea){
	
}

void creatureClean(Creature* cre6){ // creature clean new room if need be 
	if (cre6->creatureType == 1 && cre6->creatureRoom->rmState == 2){
        	cre6->creatureRoom->rmState -= 1;
    	}else if (cre6->creatureType == 2 && cre6->creatureRoom->rmState == 0){
        	cre6->creatureRoom->rmState += 1;
    	}	
}
