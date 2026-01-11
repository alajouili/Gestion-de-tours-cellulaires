#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_ANTENNES 50
#define MAX_REGION 30
#define MAX_NOM 30
#define MAX_PHONE 15
#define FTDMA_SLOTS 8
#define FTDMA_CHANNELS 4


#define PRIX_MINUTE 0.50
#define CREDIT_DEPART 10.0

#define FICHIER_ANTENNES "Towers.txt"
#define FICHIER_USERS "Users.txt"


typedef struct User {
    int userID;
    char name[MAX_NOM];
    char phone[MAX_PHONE];
    float credit;
    struct User* next;
} User;

typedef struct Antenne {
    int antenneID;
    char region[MAX_REGION];
    int frequence;
    User* userList;
} Antenne;


Antenne antennas[MAX_ANTENNES];
int antennaCount = 0;
int activeAntenna = -1;


char globalMsg[128] = "System Ready.";
int msgTimer = 0;

void ShowMessage(const char* msg) {
    strcpy(globalMsg, msg);
    msgTimer = 180; 
}



void FreeAllUsers(User *head) {
    User *courant = head;
    while (courant != NULL) {
        User *temp = courant;
        courant = courant->next;
        free(temp);
    }
}

void FreeGlobalMemory() {
    for (int i = 0; i < antennaCount; i++) {
        FreeAllUsers(antennas[i].userList);
        antennas[i].userList = NULL;
    }
    antennaCount = 0;
    activeAntenna = -1;
}

Antenne* FindAntennaByID(int id) {
    for (int i = 0; i < antennaCount; i++) {
        if (antennas[i].antenneID == id) return &antennas[i];
    }
    return NULL;
}

Antenne* FindGlobalUser(int userID, User** foundUser) {
    for (int i = 0; i < antennaCount; i++) {
        User* current = antennas[i].userList;
        while (current != NULL) {
            if (current->userID == userID) {
                *foundUser = current;
                return &antennas[i];
            }
            current = current->next;
        }
    }
    *foundUser = NULL;
    return NULL;
}

void SaveData() {
    FILE *f = fopen(FICHIER_ANTENNES, "w");
    if (!f) { ShowMessage("Error saving Towers.txt"); return; }
    fprintf(f, "%d\n", antennaCount);
    for (int i = 0; i < antennaCount; i++) {
        fprintf(f, "%d;%s;%d\n", antennas[i].antenneID, antennas[i].region, antennas[i].frequence);
    }
    fclose(f);

    f = fopen(FICHIER_USERS, "w");
    if (!f) { ShowMessage("Error saving Users.txt"); return; }
    for (int i = 0; i < antennaCount; i++) {
        User *cur = antennas[i].userList;
        while (cur != NULL) {
            fprintf(f, "%d;%d;%s;%s;%.2f\n", antennas[i].antenneID, cur->userID, cur->name, cur->phone, cur->credit);
            cur = cur->next;
        }
    }
    fclose(f);
    ShowMessage("Data Saved Successfully!");
}

void AddAntenna(int id, const char* region, int freq);
void AddUser(Antenne* a, int id, const char* name, const char* phone, float credit);

void LoadData() {
    FreeGlobalMemory();
    FILE *f = fopen(FICHIER_ANTENNES, "r");
    if (!f) { ShowMessage("Towers.txt not found."); return; }
    int n, id, freq;
    char reg[MAX_REGION];
    if (fscanf(f, "%d\n", &n) == 1) {
        for (int i = 0; i < n && i < MAX_ANTENNES; i++) {
            if (fscanf(f, "%d;%29[^;];%d\n", &id, reg, &freq) == 3) {
                AddAntenna(id, reg, freq);
            }
        }
    }
    fclose(f);

    f = fopen(FICHIER_USERS, "r");
    if (!f) { ShowMessage("Users loaded (no Users.txt found)."); return; }
    int aid, uid;
    char nom[MAX_NOM], tel[MAX_PHONE];
    float cred;
    while (fscanf(f, "%d;%d;%29[^;];%14[^;];%f\n", &aid, &uid, nom, tel, &cred) == 5) {
        Antenne *a = FindAntennaByID(aid);
        if (a) AddUser(a, uid, nom, tel, cred);
    }
    fclose(f);
    ShowMessage("Data Loaded Successfully!");
}

void AddAntenna(int id, const char* region, int freq) {
    if (antennaCount >= MAX_ANTENNES) return;
    antennas[antennaCount] = (Antenne){ id, "", freq, NULL };
    strcpy(antennas[antennaCount].region, region);
    antennaCount++;
}

void DeleteAntenna(int index) {
    if (index < 0 || index >= antennaCount) return;
    FreeAllUsers(antennas[index].userList);
    for (int i = index; i < antennaCount - 1; i++)
        antennas[i] = antennas[i + 1];
    antennaCount--;
    activeAntenna = -1;
}

void AddUser(Antenne* a, int id, const char* name, const char* phone, float credit) {
    User* u = a->userList;
    while(u) { if(u->userID == id) return; u=u->next; } 
    User* newUser = (User*)malloc(sizeof(User));
    *newUser = (User){ id, "", "", credit, a->userList };
    strcpy(newUser->name, name);
    strcpy(newUser->phone, phone);
    a->userList = newUser;
}

void DeleteUser(Antenne* a, int id) {
    User *c = a->userList, *p = NULL;
    while (c) {
        if (c->userID == id) {
            if (p) p->next = c->next;
            else a->userList = c->next;
            free(c);
            return;
        }
        p = c;
        c = c->next;
    }
}

void SortUsersByName(Antenne* a) {
    if(!a || !a->userList) return;
    int swapped;
    User *ptr1;
    User *lptr = NULL;

    do {
        swapped = 0;
        ptr1 = a->userList;
        while (ptr1->next != lptr) {
            if (strcmp(ptr1->name, ptr1->next->name) > 0) {
                int tempID = ptr1->userID;
                char tempName[MAX_NOM]; strcpy(tempName, ptr1->name);
                char tempPhone[MAX_PHONE]; strcpy(tempPhone, ptr1->phone);
                float tempCredit = ptr1->credit;

                ptr1->userID = ptr1->next->userID;
                strcpy(ptr1->name, ptr1->next->name);
                strcpy(ptr1->phone, ptr1->next->phone);
                ptr1->credit = ptr1->next->credit;

                ptr1->next->userID = tempID;
                strcpy(ptr1->next->name, tempName);
                strcpy(ptr1->next->phone, tempPhone);
                ptr1->next->credit = tempCredit;
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    ShowMessage("Users sorted alphabetically.");
}

void TransferUser(int userID, int destAntennaID) {
    User *u;
    Antenne* src = FindGlobalUser(userID, &u);
    Antenne* dest = FindAntennaByID(destAntennaID);

    if (!src) { ShowMessage("User not found."); return; }
    if (!dest) { ShowMessage("Destination Antenna not found."); return; }
    if (src == dest) { ShowMessage("Source and Destination are the same."); return; }

    int uid = u->userID;
    char uname[MAX_NOM]; strcpy(uname, u->name);
    char uphone[MAX_PHONE]; strcpy(uphone, u->phone);
    float ucred = u->credit;

    DeleteUser(src, uid);
    AddUser(dest, uid, uname, uphone, ucred);
    ShowMessage("Transfer Successful (Handover).");
}

void ExecuteCall(int callerID, int receiverID, int duration) {
    User *caller, *receiver;
    Antenne *antCaller = FindGlobalUser(callerID, &caller);
    Antenne *antReceiver = FindGlobalUser(receiverID, &receiver);

    if (!antCaller) { ShowMessage("Caller ID not found."); return; }
    if (!antReceiver) { ShowMessage("Receiver ID not found."); return; }

    float cost = duration * PRIX_MINUTE;

    if (caller->credit >= cost) {
        caller->credit -= cost;
        char buff[100];
        snprintf(buff, 100, "Success! Cost: %.2f$. New Balance: %.2f$", cost, caller->credit);
        ShowMessage(buff);
    } else {
        char buff[100];
        snprintf(buff, 100, "Failed! Needed: %.2f$, Has: %.2f$", cost, caller->credit);
        ShowMessage(buff);
    }
}

void RechargeUser(int id, float amount) {
    User* u;
    if (FindGlobalUser(id, &u)) {
        u->credit += amount;
        ShowMessage("Recharge Successful.");
    } else {
        ShowMessage("User not found for recharge.");
    }
}

void DrawFTDMA(int x, int y, Antenne* a) {
    if (!a) return;
    int cell = 32;
    User* u = a->userList;

    DrawText("FTDMA Grid (Time Slots vs Channels)", x, y - 25, 16, DARKGRAY);

    for (int f = 0; f < FTDMA_CHANNELS; f++) {
        DrawText(TextFormat("F%d", f + 1), x - 25, y + f * cell + 8, 12, GRAY);
        for (int t = 0; t < FTDMA_SLOTS; t++) {
            Rectangle r = { x + t * cell, y + f * cell, cell - 2, cell - 2 };
            DrawRectangleRec(r, u ? SKYBLUE : LIGHTGRAY);
            if (u) {
                DrawText(TextFormat("U%d", u->userID), r.x + 4, r.y + 10, 10, BLACK);
                u = u->next;
            }
        }
    }
}

void DrawStatsOverlay(bool *show) {
    DrawRectangle(0, 0, 1150, 720, Fade(BLACK, 0.8f));
    Rectangle box = { 100, 100, 950, 520 };
    DrawRectangleRec(box, RAYWHITE);
    DrawRectangleLinesEx(box, 3, DARKBLUE);
    
    DrawText("NETWORK STATISTICS", box.x + 20, box.y + 20, 24, DARKBLUE);
    
    int totalUsers = 0;
    int y = box.y + 70;
    int maxU = -1, idMax = -1;

    for(int i=0; i<antennaCount; i++) {
        int count = 0;
        User* u = antennas[i].userList;
        while(u) { count++; u=u->next; }
        totalUsers += count;
        
        float charge = ((float)count / (FTDMA_SLOTS * FTDMA_CHANNELS)) * 100;
        DrawText(TextFormat("Antenna %d (%s): %d users -> Load: %.2f%%", 
                 antennas[i].antenneID, antennas[i].region, count, charge), 
                 box.x + 30, y, 20, (charge > 80)? RED : BLACK);
        
        if(count > maxU) { maxU = count; idMax = antennas[i].antenneID; }
        y += 30;
    }

    y += 20;
    DrawLine(box.x+20, y, box.x+930, y, LIGHTGRAY);
    y += 20;
    DrawText(TextFormat("Total Network Users: %d", totalUsers), box.x + 30, y, 22, DARKGREEN);
    if(idMax != -1)
        DrawText(TextFormat("Busiest Antenna: ID %d (%d users)", idMax, maxU), box.x + 30, y+30, 22, DARKGREEN);

    if (GuiButton((Rectangle){box.x + 800, box.y + 470, 120, 40}, "CLOSE")) *show = false;
}

int main(void) {
    InitWindow(1150, 720, "Telecom Network Manager");
    SetTargetFPS(60);

    AddAntenna(1, "Paris", 1800);
    AddAntenna(2, "Marseille", 900);

    bool showAddUser = false, showAddAntenna = false;
    bool showCallSim = false, showRecharge = false, showTransfer = false, showStats = false;

    char uname[32] = "", phone[16] = "";
    bool editName = false, editPhone = false;

    char region[32] = "", freq[8] = "";
    bool editRegion = false, editFreq = false;

    char callSrc[10]="", callDest[10]="", callDur[10]="";
    bool eSrc=0, eDest=0, eDur=0;
    
    char rechargeID[10]="", rechargeAmt[10]="";
    bool eRecID=0, eRecAmt=0;

    char transID[10]="", transDest[10]="";
    bool eTransID=0, eTransDest=0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, 1150, 60, DARKBLUE);
        DrawText("TELECOM NETWORK MANAGER", 20, 18, 24, WHITE);
        
        if (msgTimer > 0) {
            DrawRectangle(400, 10, 600, 40, DARKGREEN);
            DrawText(globalMsg, 420, 20, 20, WHITE);
            msgTimer--;
        }

        if (GuiButton((Rectangle){900, 15, 60, 30}, "SAVE")) SaveData();
        if (GuiButton((Rectangle){970, 15, 60, 30}, "LOAD")) LoadData();
        if (GuiButton((Rectangle){1040, 15, 80, 30}, "STATS")) showStats = true;

        DrawRectangle(0, 60, 260, 660, Fade(GRAY, 0.1f));
        DrawText("Antennas", 20, 80, 18, DARKGRAY);

        for (int i = 0; i < antennaCount; i++) {
            if (GuiButton((Rectangle){20, 110 + i * 40, 220, 34},
                TextFormat("ID %d | %s", antennas[i].antenneID, antennas[i].region))) {
                activeAntenna = i;
            }
        }

        int btnY = 500;
        if (GuiButton((Rectangle){20, btnY, 220, 30}, "+ Add Antenna")) showAddAntenna = true;
        btnY += 35;
        if (GuiButton((Rectangle){20, btnY, 220, 30}, "Simulate Call")) showCallSim = true;
        btnY += 35;
        if (GuiButton((Rectangle){20, btnY, 220, 30}, "Recharge Credit")) showRecharge = true;
        btnY += 35;
        if (GuiButton((Rectangle){20, btnY, 220, 30}, "Handover (Transfer)")) showTransfer = true;

        if (activeAntenna != -1 &&
            GuiButton((Rectangle){20, 670, 220, 36}, "Delete Active Antenna")) {
            DeleteAntenna(activeAntenna);
        }

        if (activeAntenna != -1) {
            Antenne* a = &antennas[activeAntenna];
            DrawText(TextFormat("Region: %s | %d MHz", a->region, a->frequence), 300, 80, 22, BLACK);
            
            if (GuiButton((Rectangle){900, 80, 120, 25}, "Sort by Name")) SortUsersByName(a);

            User* u = a->userList;
            int y = 130;
            DrawText("ID    Name         Phone        Credit", 300, 110, 12, GRAY);
            
            while (u && y < 380) {
                DrawText(TextFormat("%d    %-12s %-12s %.2f$",
                    u->userID, u->name, u->phone, u->credit),
                    300, y, 14, BLACK);

                if (GuiButton((Rectangle){900, y - 4, 60, 22}, "DEL"))
                    DeleteUser(a, u->userID);

                u = u->next;
                y += 26;
            }

            if (GuiButton((Rectangle){900, 390, 120, 30}, "+ Add User")) showAddUser = true;
            DrawFTDMA(300, 450, a);
        } else {
            DrawText("Select an antenna to manage users.", 450, 300, 20, GRAY);
        }


        if (showAddUser) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
            
            Rectangle box = { 430, 230, 300, 260 };
            DrawRectangleRec(box, RAYWHITE);
            DrawRectangleLinesEx(box, 2, DARKGRAY);
            DrawText("Add New User", box.x + 10, box.y + 10, 20, DARKBLUE);
            
            DrawText("Name:", box.x + 20, box.y + 45, 12, GRAY);
            if (GuiTextBox((Rectangle){box.x + 20, box.y + 60, 260, 30}, uname, 30, editName)) {
                editName = !editName;
            }
            
            DrawText("Phone Number:", box.x + 20, box.y + 105, 12, GRAY);
            if (GuiTextBox((Rectangle){box.x + 20, box.y + 120, 260, 30}, phone, 15, editPhone)) {
                editPhone = !editPhone;
            }
            
            if (GuiButton((Rectangle){box.x + 20, box.y + 200, 120, 30}, "ADD")) {
                AddUser(&antennas[activeAntenna], GetRandomValue(100, 999), uname, phone, CREDIT_DEPART);
                
                uname[0] = '\0'; 
                phone[0] = '\0';
                editName = false;
                editPhone = false;
                showAddUser = false;
            }
            
            if (GuiButton((Rectangle){box.x + 160, box.y + 200, 120, 30}, "CANCEL")) {
                showAddUser = false;
                editName = false;
                editPhone = false;
            }
        }

        if (showAddAntenna) {
            DrawRectangle(0,0,1150,720,Fade(BLACK,0.5f));
            Rectangle box = {430, 230, 300, 260};
            DrawRectangleRec(box, RAYWHITE); DrawRectangleLinesEx(box, 2, DARKGRAY);
            DrawText("Add Antenna", box.x+10, box.y+10, 20, DARKBLUE);
            
            DrawText("Region Name", box.x+20, box.y+40, 10, GRAY);
            if (GuiTextBox((Rectangle){box.x+20, box.y+55, 260, 30}, region, 30, editRegion)) editRegion = !editRegion;

            DrawText("Freq", box.x+20, box.y+90, 10, GRAY);
            if (GuiTextBox((Rectangle){box.x+20, box.y+105, 260, 30}, freq, 8, editFreq)) editFreq = !editFreq;

            if (GuiButton((Rectangle){box.x+20, box.y+200, 120, 30}, "Create")) {
                AddAntenna(GetRandomValue(10, 99), region, atoi(freq));
                region[0]=0; freq[0]=0; showAddAntenna=false;
            }
            if (GuiButton((Rectangle){box.x+160, box.y+200, 120, 30}, "Cancel")) showAddAntenna=false;
        }

        if (showCallSim) {
            DrawRectangle(0,0,1150,720,Fade(BLACK,0.5f));
            Rectangle box = {400, 200, 350, 300};
            DrawRectangleRec(box, RAYWHITE); DrawRectangleLinesEx(box, 2, DARKBLUE);
            DrawText("SIMULATE CALL", box.x+100, box.y+10, 20, DARKBLUE);

            DrawText("Caller ID:", box.x+20, box.y+50, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+120, box.y+45, 200, 25}, callSrc, 10, eSrc)) eSrc = !eSrc;

            DrawText("Receiver ID:", box.x+20, box.y+90, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+120, box.y+85, 200, 25}, callDest, 10, eDest)) eDest = !eDest;

            DrawText("Duration(m):", box.x+20, box.y+130, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+120, box.y+125, 200, 25}, callDur, 10, eDur)) eDur = !eDur;

            if (GuiButton((Rectangle){box.x+40, box.y+200, 120, 30}, "CALL")) {
                ExecuteCall(atoi(callSrc), atoi(callDest), atoi(callDur));
                showCallSim = false;
            }
            if (GuiButton((Rectangle){box.x+190, box.y+200, 120, 30}, "CLOSE")) showCallSim = false;
        }

        if (showRecharge) {
            DrawRectangle(0,0,1150,720,Fade(BLACK,0.5f));
            Rectangle box = {450, 250, 300, 200};
            DrawRectangleRec(box, RAYWHITE); DrawRectangleLinesEx(box, 2, GREEN);
            DrawText("RECHARGE CREDIT", box.x+60, box.y+10, 18, DARKGREEN);

            DrawText("User ID:", box.x+20, box.y+50, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+100, box.y+45, 150, 25}, rechargeID, 10, eRecID)) eRecID = !eRecID;

            DrawText("Amount $:", box.x+20, box.y+90, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+100, box.y+85, 150, 25}, rechargeAmt, 10, eRecAmt)) eRecAmt = !eRecAmt;

            if (GuiButton((Rectangle){box.x+30, box.y+140, 100, 30}, "ADD")) {
                RechargeUser(atoi(rechargeID), atof(rechargeAmt));
                showRecharge = false;
            }
            if (GuiButton((Rectangle){box.x+170, box.y+140, 100, 30}, "CANCEL")) showRecharge = false;
        }

        if (showTransfer) {
            DrawRectangle(0,0,1150,720,Fade(BLACK,0.5f));
            Rectangle box = {450, 250, 300, 220};
            DrawRectangleRec(box, RAYWHITE); DrawRectangleLinesEx(box, 2, MAROON);
            DrawText("HANDOVER USER", box.x+70, box.y+10, 18, MAROON);

            DrawText("User ID:", box.x+20, box.y+50, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+120, box.y+45, 150, 25}, transID, 10, eTransID)) eTransID = !eTransID;

            DrawText("Dest. Ant ID:", box.x+20, box.y+90, 14, GRAY);
            if (GuiTextBox((Rectangle){box.x+120, box.y+85, 150, 25}, transDest, 10, eTransDest)) eTransDest = !eTransDest;

            if (GuiButton((Rectangle){box.x+30, box.y+150, 100, 30}, "MOVE")) {
                TransferUser(atoi(transID), atoi(transDest));
                showTransfer = false;
            }
            if (GuiButton((Rectangle){box.x+170, box.y+150, 100, 30}, "CANCEL")) showTransfer = false;
        }

        if (showStats) DrawStatsOverlay(&showStats);

        EndDrawing();
    }

    FreeGlobalMemory();
    CloseWindow();
    return 0;
}