#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_ROOMS 20
#define FILENAME "hotel.dat"
#define GUEST_NAME_LEN 100
#define ONE_DAY_SECONDS (24 * 60 * 60)

typedef struct {
    int room_number;
    int is_occupied; // 0 = no, 1 = yes
    char guest_name[GUEST_NAME_LEN];
    time_t checkout_time;
} Room;

// Global hotel state
Room hotel[TOTAL_ROOMS];

// Initialize rooms (or load from file)
void initialize_hotel() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp) {
        // File exists, load it
        fread(hotel, sizeof(Room), TOTAL_ROOMS, fp);
        fclose(fp);
        printf("Loaded hotel data from %s.\n", FILENAME);
    } else {
        // No file, create fresh state
        printf("No existing data, initializing fresh hotel state.\n");
        for (int i = 0; i < TOTAL_ROOMS; i++) {
            hotel[i].room_number = 101 + i;
            hotel[i].is_occupied = 0;
            strcpy(hotel[i].guest_name, "N/A");
            hotel[i].checkout_time = 0;
        }
    }
}

// Save the hotel state to disk
void save_hotel_data() {
    FILE *fp = fopen(FILENAME, "wb");
    if (fp) {
        fwrite(hotel, sizeof(Room), TOTAL_ROOMS, fp);
        fclose(fp);
        printf("Data saved to %s.\n", FILENAME);
    } else {
        perror("Error saving data");
    }
}

// Auto-checkout guests whose time is up
void check_expired_stays() {
    time_t now = time(NULL);
    int expired_count = 0;
    for (int i = 0; i < TOTAL_ROOMS; i++) {
        if (hotel[i].is_occupied && difftime(now, hotel[i].checkout_time) > 0) {
            printf("Auto-checking out Room %d (Guest: %s)\n", hotel[i].room_number, hotel[i].guest_name);
            hotel[i].is_occupied = 0;
            strcpy(hotel[i].guest_name, "N/A");
            expired_count++;
        }
    }
    if (expired_count > 0) save_hotel_data();
}

void view_rooms() {
    printf("\n--- Hotel Room Status ---\n");
    for (int i = 0; i < TOTAL_ROOMS; i++) {
        printf("Room %d: [%s]", hotel[i].room_number, hotel[i].is_occupied ? "OCCUPIED" : "Available");
        if (hotel[i].is_occupied) {
            char *time_str = ctime(&hotel[i].checkout_time);
            time_str[strcspn(time_str, "\n")] = 0;
            printf(" - Guest: %s (Checkout: %s)\n", hotel[i].guest_name, time_str);
        } else {
            printf("\n");
        }
    }
    printf("-------------------------\n");
}

void check_in() {
    int room_num;
    printf("Enter room number (101-%d): ", 100 + TOTAL_ROOMS);
    scanf("%d", &room_num);
    
    int i = room_num - 101; // Array index
    if (i < 0 || i >= TOTAL_ROOMS) {
        printf("Invalid room number.\n");
        return;
    }
    
    if (hotel[i].is_occupied) {
        printf("Room %d is already occupied.\n", room_num);
    } else {
        hotel[i].is_occupied = 1;
        printf("Enter guest name: ");
        getchar(); // consume newline
        fgets(hotel[i].guest_name, GUEST_NAME_LEN, stdin);
        hotel[i].guest_name[strcspn(hotel[i].guest_name, "\n")] = 0;
        
        // Set checkout for 24 hours from now
        hotel[i].checkout_time = time(NULL) + ONE_DAY_SECONDS; 
        
        printf("Guest %s checked into Room %d.\n", hotel[i].guest_name, room_num);
        save_hotel_data();
    }
}

void check_out() {
    int room_num;
    printf("Enter room number to check out: ");
    scanf("%d", &room_num);
    
    int i = room_num - 101;
    if (i < 0 || i >= TOTAL_ROOMS) {
        printf("Invalid room number.\n");
        return;
    }

    if (!hotel[i].is_occupied) {
        printf("Room %d is already empty.\n", room_num);
    } else {
        printf("Checking out Guest %s from Room %d.\n", hotel[i].guest_name, room_num);
        hotel[i].is_occupied = 0;
        strcpy(hotel[i].guest_name, "N/A");
        hotel[i].checkout_time = 0;
        save_hotel_data();
    }
}

int main() {
    initialize_hotel();
    check_expired_stays();
    
    int choice;
    while(1) {
        printf("\nHotel Management System\n");
        printf("1. View Rooms\n2. Check In\n3. Check Out\n4. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // clear invalid
            printf("Invalid input.\n");
            continue;
        }
        
        switch(choice) {
            case 1: view_rooms(); break;
            case 2: check_in(); break;
            case 3: check_out(); break;
            case 4: exit(0);
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
