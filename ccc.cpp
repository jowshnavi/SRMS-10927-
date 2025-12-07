#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINE 256
typedef struct {
    int roll;
    char name[50];
    char dept[30];
    float marks_overall; 
} Student;

typedef struct {
    int roll;
    int presentDays;
    int totalDays;
} Attendance;

typedef struct {
    int roll;
    int sub1, sub2, sub3, sub4, sub5;
    float percent;
} MarksRec;

typedef struct {
    int roll;
    float totalFee;
    float paidFee;
} FeeRec;

char currentUsername[50];
char currentRole[20];
int currentStudentRoll = -1;

int fileExists(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

void pause_console() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n'); 
    getchar();
}

void getLineInput(char *buf, int size) {
    int c, i=0;
    while ((c = getchar()) != '\n' && c != EOF && i < size-1) {
        buf[i++] = (char)c;
    }
    buf[i] = '\0';
}
int parseUserLine(const char *line, char *u, char *p, char *r, int *roll) {
    *roll = -1;
    int cnt = sscanf(line, "%49s %49s %19s %d", u, p, r, roll);
    return (cnt >= 3);
}
int changePasswordFile(const char *username, const char *newpass) {
    FILE *fp = fopen("users.txt", "r");
    FILE *tmp = fopen("users_tmp.txt", "w");
    if (!fp || !tmp) { 
	   if (fp) 
	       fclose(fp); 
		   if (tmp) 
		       fclose(tmp); 
			   return 0; 
	}

    char line[MAXLINE], u[50], p[50], r[20];
    int roll;
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (!parseUserLine(line,u,p,r,&roll)) {
            fputs(line, tmp);
            continue;
        }
        if (strcmp(u, username) == 0) {
            if (roll != -1) fprintf(tmp, "%s %s %s %d\n", u, newpass, r, roll);
            else fprintf(tmp, "%s %s %s\n", u, newpass, r);
            found = 1;
        } else {
            if (roll != -1) fprintf(tmp, "%s %s %s %d\n", u, p, r, roll);
            else fprintf(tmp, "%s %s %s\n", u, p, r);
        }
    }
    fclose(fp); 
	fclose(tmp);
    if (found) {
        remove("users.txt");
        rename("users_tmp.txt", "users.txt");
        return 1;
    } else {
        remove("users_tmp.txt");
        return 0;
    }
}

void addNewUser() {
    char uname[50], pass[50], role[20];
    int roll = -1;

    printf("\n--- Add New User ---\n");
    printf("Enter username: ");
    scanf("%s", uname);
    printf("Enter password: ");
    scanf("%s", pass);
    printf("Enter role (ADMIN / STAFF / STUDENT / USER): ");
    scanf("%s", role);
    if (strcmp(role,"STUDENT")==0) {
        printf("Enter student's roll to link (integer): ");
        scanf("%d", &roll);
    }

    FILE *fp = fopen("users.txt","a");
    if (!fp) { printf("Error opening users.txt for append.\n"); return; }
    if (roll != -1) fprintf(fp, "%s %s %s %d\n", uname, pass, role, roll);
    else fprintf(fp, "%s %s %s\n", uname, pass, role);
    fclose(fp);
    printf("User added.\n");
}

void viewAllUsers() {
    char line[MAXLINE];
    FILE *fp = fopen("users.txt","r");
    if (!fp) { printf("No users file found.\n"); return; }
    printf("\n--- Users ---\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

int login() {
    char username[50], password[50];
    char line[MAXLINE], u[50], p[50], r[20];
    int roll;

    printf("\n==== LOGIN ====\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    FILE *fp = fopen("users.txt","r");
    if (!fp) { printf("users.txt not found. Creating default admin...\n"); return 0; }

    while (fgets(line, sizeof(line), fp)) {
        if (!parseUserLine(line, u, p, r, &roll)) continue;
        if (strcmp(u, username) == 0) {
            if (strcmp(p, password) == 0) {
                strcpy(currentUsername, u);
                strcpy(currentRole, r);
                currentStudentRoll = (roll != -1) ? roll : -1;
                fclose(fp);
                return 1;
            } else {
                fclose(fp);
                return -1;
            }
        }
    }
    fclose(fp);
    return -2;
}

void addStudentRecord() {
    Student s;
    FILE *fp = fopen("students.dat","ab");
    if (!fp) { printf("Cannot open students.dat\n"); return; }
    printf("\n--- Add Student ---\n");
    printf("Roll: "); scanf("%d", &s.roll);
    printf("Name (no spaces): "); scanf("%s", s.name);
    printf("Dept: "); scanf("%s", s.dept);
    s.marks_overall = 0.0f;
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("Student added.\n");
}

void viewAllStudents() {
    Student s;
    FILE *fp = fopen("students.dat","rb");
    if (!fp) { printf("No student records.\n"); return; }
    printf("\n--- Students ---\n");
    while (fread(&s, sizeof(Student), 1, fp)) {
        printf("Roll: %d | Name: %s | Dept: %s | SummaryMarks: %.2f\n",
               s.roll, s.name, s.dept, s.marks_overall);
    }
    fclose(fp);
}

void searchStudentByRoll() {
    int roll;
    printf("Enter roll to search: "); scanf("%d", &roll);
    Student s;
    FILE *fp = fopen("students.dat","rb");
    int found=0;
    if (!fp) { printf("No students.\n"); return; }
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) {
            printf("\nFound: Roll: %d | Name: %s | Dept: %s | SummaryMarks: %.2f\n",
                   s.roll, s.name, s.dept, s.marks_overall);
            found = 1; break;
        }
    }
    if (!found) printf("Not found.\n");
    fclose(fp);
}

void updateStudentByRoll() {
    int roll; 
	printf("Enter roll to update: "); 
	scanf("%d", &roll);
    FILE *fp = fopen("students.dat","rb+");
    Student s; 
	int found=0;
    if (!fp) { 
	   printf("No students.\n"); 
	   return; 
	}
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) {
            printf("Enter new Name (no spaces): "); 
			scanf("%s", s.name);
            printf("Enter new Dept: "); 
			scanf("%s", s.dept);
            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            found = 1; 
			printf("Updated.\n"); 
			break;
        }
    }
    if (!found) printf("Not found.\n");
    fclose(fp);
}

void deleteStudentByRoll() {
    int roll; printf("Enter roll to delete: "); scanf("%d", &roll);
    FILE *fp = fopen("students.dat","rb");
    FILE *tmp = fopen("students_tmp.dat","wb");
    Student s; int found=0;
    if (!fp || !tmp) { if (fp) fclose(fp); if (tmp) fclose(tmp); printf("File error.\n"); return; }
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) { found = 1; /* skip */ }
        else fwrite(&s, sizeof(Student), 1, tmp);
    }
    fclose(fp); fclose(tmp);
    remove("students.dat"); rename("students_tmp.dat","students.dat");
    if (found) printf("Deleted.\n"); else printf("Not found.\n");
}

void markAttendanceForRoll() {
    int roll; printf("Enter roll to mark present (increments total and present): "); scanf("%d", &roll);
    FILE *fp = fopen("attendance.dat","rb+");
    Attendance a; int found=0;
    if (!fp) {
        fp = fopen("attendance.dat","wb");
        if (!fp) { printf("Cannot open attendance file.\n"); return; }
        fclose(fp);
        fp = fopen("attendance.dat","rb+");
    }
    while (fread(&a, sizeof(Attendance), 1, fp)) {
        if (a.roll == roll) {
            a.totalDays += 1;
            a.presentDays += 1;
            fseek(fp, -sizeof(Attendance), SEEK_CUR);
            fwrite(&a, sizeof(Attendance), 1, fp);
            found = 1;
            printf("Marked present. Now %d/%d\n", a.presentDays, a.totalDays);
            break;
        }
    }
    if (!found) {
        a.roll = roll; a.totalDays = 1; a.presentDays = 1;
        fwrite(&a, sizeof(Attendance), 1, fp);
        printf("Attendance created and marked present 1/1\n");
    }
    fclose(fp);
}

void markAbsentForRoll() {
    int roll; printf("Enter roll to mark absent (increments total only): "); scanf("%d", &roll);
    FILE *fp = fopen("attendance.dat","rb+");
    Attendance a; int found=0;
    if (!fp) { fp = fopen("attendance.dat","wb"); if (!fp) { printf("Cannot open attendance file.\n"); return; } fclose(fp); fp = fopen("attendance.dat","rb+"); }
    while (fread(&a, sizeof(Attendance), 1, fp)) {
        if (a.roll == roll) {
            a.totalDays += 1;
            fseek(fp, -sizeof(Attendance), SEEK_CUR);
            fwrite(&a, sizeof(Attendance), 1, fp);
            found = 1;
            printf("Marked absent. Now %d/%d\n", a.presentDays, a.totalDays);
            break;
        }
    }
    if (!found) {
        a.roll = roll; a.totalDays = 1; a.presentDays = 0;
        fwrite(&a, sizeof(Attendance), 1, fp);
        printf("Attendance created and marked absent 0/1\n");
    }
    fclose(fp);
}

void viewAllAttendance() {
    Attendance a; FILE *fp = fopen("attendance.dat","rb");
    if (!fp) { printf("No attendance data.\n"); return; }
    printf("\n--- Attendance ---\n");
    while (fread(&a, sizeof(Attendance), 1, fp)) {
        printf("Roll: %d -> %d/%d (%.2f%%)\n", a.roll, a.presentDays, a.totalDays,
               a.totalDays? (100.0f * a.presentDays / a.totalDays) : 0.0f);
    }
    fclose(fp);
}

void viewAttendanceByRollInteractive() {
    int roll; printf("Enter roll: "); scanf("%d", &roll);
    Attendance a; FILE *fp = fopen("attendance.dat","rb");
    int found=0;
    if (!fp) { printf("No attendance data.\n"); return; }
    while (fread(&a, sizeof(Attendance), 1, fp)) {
        if (a.roll == roll) {
            printf("Roll %d -> %d/%d (%.2f%%)\n", a.roll, a.presentDays, a.totalDays,
                   a.totalDays? (100.0f * a.presentDays / a.totalDays) : 0.0f);
            found=1; break;
        }
    }
    if (!found) printf("No attendance for roll %d\n", roll);
    fclose(fp);
}

void addOrUpdateMarks() {
    int roll; printf("Enter student roll: "); scanf("%d", &roll);
    FILE *fp = fopen("marks.dat","rb+");
    MarksRec m; int found=0;
    if (!fp) { fp = fopen("marks.dat","wb"); if (!fp) { printf("Cannot open marks file.\n"); return; } fclose(fp); fp = fopen("marks.dat","rb+"); }
    while (fread(&m, sizeof(MarksRec), 1, fp)) {
        if (m.roll == roll) {
            printf("Existing found. Enter new marks for 5 subjects (int 0-100): ");
            scanf("%d %d %d %d %d", &m.sub1, &m.sub2, &m.sub3, &m.sub4, &m.sub5);
            m.percent = (m.sub1 + m.sub2 + m.sub3 + m.sub4 + m.sub5) / 5.0f;
            fseek(fp, -sizeof(MarksRec), SEEK_CUR);
            fwrite(&m, sizeof(MarksRec), 1, fp);
            found = 1; printf("Marks updated.\n"); break;
        }
    }
    if (!found) {
        m.roll = roll;
        printf("Enter marks for 5 subjects (int 0-100): ");
        scanf("%d %d %d %d %d", &m.sub1, &m.sub2, &m.sub3, &m.sub4, &m.sub5);
        m.percent = (m.sub1 + m.sub2 + m.sub3 + m.sub4 + m.sub5) / 5.0f;
        fwrite(&m, sizeof(MarksRec), 1, fp);
        printf("Marks added.\n");
    }
    fclose(fp);
}

void viewAllMarks() {
    MarksRec m; FILE *fp = fopen("marks.dat","rb");
    if (!fp) { printf("No marks data.\n"); return; }
    printf("\n--- Marks ---\n");
    while (fread(&m, sizeof(MarksRec), 1, fp)) {
        printf("Roll: %d | marks: %d %d %d %d %d | %%: %.2f | Result: %s\n",
               m.roll, m.sub1, m.sub2, m.sub3, m.sub4, m.sub5, m.percent,
               (m.sub1<40||m.sub2<40||m.sub3<40||m.sub4<40||m.sub5<40) ? "FAIL" : "PASS");
    }
    fclose(fp);
}

void viewMarksForRollInteractive() {
    int roll; printf("Enter roll: "); scanf("%d", &roll);
    MarksRec m; FILE *fp = fopen("marks.dat","rb");
    int found=0;
    if (!fp) { printf("No marks data.\n"); return; }
    while (fread(&m, sizeof(MarksRec), 1, fp)) {
        if (m.roll == roll) {
            printf("Roll: %d | marks: %d %d %d %d %d | %%: %.2f | Result: %s\n",
                   m.roll, m.sub1, m.sub2, m.sub3, m.sub4, m.sub5, m.percent,
                   (m.sub1<40||m.sub2<40||m.sub3<40||m.sub4<40||m.sub5<40) ? "FAIL" : "PASS");
            found=1; break;
        }
    }
    if (!found) printf("No marks for roll %d\n", roll);
    fclose(fp);
}
void addOrUpdateFeeRecord() {
    int roll; printf("Enter roll: "); scanf("%d", &roll);
    FILE *fp = fopen("fees.dat","rb+");
    FeeRec frec; int found=0;
    if (!fp) { fp = fopen("fees.dat","wb"); if (!fp) { printf("Cannot open fees file.\n"); return; } fclose(fp); fp = fopen("fees.dat","rb+"); }
    while (fread(&frec, sizeof(FeeRec), 1, fp)) {
        if (frec.roll == roll) {
            printf("Existing: total=%.2f paid=%.2f due=%.2f\n", frec.totalFee, frec.paidFee, frec.totalFee - frec.paidFee);
            printf("Enter new total fee: "); scanf("%f", &frec.totalFee);
            printf("Enter paid amount now (will add to existing paid): "); float add; scanf("%f",&add); frec.paidFee += add;
            fseek(fp, -sizeof(FeeRec), SEEK_CUR);
            fwrite(&frec, sizeof(FeeRec), 1, fp);
            found=1; printf("Fee updated.\n"); break;
        }
    }
    if (!found) {
        frec.roll = roll;
        printf("Enter total fee: "); scanf("%f", &frec.totalFee);
        printf("Enter paid amount now: "); scanf("%f", &frec.paidFee);
        fwrite(&frec, sizeof(FeeRec), 1, fp);
        printf("Fee record added.\n");
    }
    fclose(fp);
}

void viewAllFees() {
    FeeRec frec; FILE *fp = fopen("fees.dat","rb");
    if (!fp) { printf("No fees data.\n"); return; }
    printf("\n--- Fees ---\n");
    while (fread(&frec, sizeof(FeeRec), 1, fp)) {
        printf("Roll: %d | Total: %.2f | Paid: %.2f | Due: %.2f\n",
               frec.roll, frec.totalFee, frec.paidFee, frec.totalFee - frec.paidFee);
    }
    fclose(fp);
}

void viewFeeForRollInteractive() {
    int roll; printf("Enter roll: "); scanf("%d", &roll);
    FeeRec frec; FILE *fp = fopen("fees.dat","rb");
    int found=0;
    if (!fp) { printf("No fees data.\n"); return; }
    while (fread(&frec, sizeof(FeeRec), 1, fp)) {
        if (frec.roll == roll) {
            printf("Roll: %d | Total: %.2f | Paid: %.2f | Due: %.2f\n",
                   frec.roll, frec.totalFee, frec.paidFee, frec.totalFee - frec.paidFee);
            found = 1; break;
        }
    }
    if (!found) printf("No fee record for roll %d\n", roll);
    fclose(fp);
}

void addNotice() {
    char buf[200];
    getchar(); 
    printf("Enter notice (single line): ");
    getLineInput(buf, sizeof(buf));
    FILE *fp = fopen("notices.txt","a");
    if (!fp) { printf("Cannot open notices.txt\n"); return; }
    fprintf(fp, "%s\n", buf);
    fclose(fp);
    printf("Notice added.\n");
}

void viewNotices() {
    char line[300]; FILE *fp = fopen("notices.txt","r");
    if (!fp) { printf("No notices.\n"); return; }
    printf("\n--- Notices ---\n");
    int idx=1;
    while (fgets(line, sizeof(line), fp)) {
        printf("%d) %s", idx++, line);
    }
    fclose(fp);
}

void deleteNoticeByIndex() {
    viewNotices();
    printf("Enter notice index to delete: ");
    int idx; scanf("%d",&idx);
    FILE *fp = fopen("notices.txt","r");
    FILE *tmp = fopen("notices_tmp.txt","w");
    if (!fp || !tmp) { if (fp) fclose(fp); if (tmp) fclose(tmp); printf("No notices or error.\n"); return; }
    char line[300]; int i=1;
    while (fgets(line,sizeof(line),fp)) {
        if (i != idx) fputs(line,tmp);
        i++;
    }
    fclose(fp); fclose(tmp);
    remove("notices.txt"); rename("notices_tmp.txt","notices.txt");
    printf("Deleted notice %d if existed.\n", idx);
}

void addOrUpdateTimetable() {
    char buf[500];
    getchar();
    printf("Enter timetable content (multiple lines). End with a single line containing only END\n");
    FILE *fp = fopen("timetable.txt","w");
    if (!fp) { printf("Cannot open timetable.txt for writing.\n"); return; }
    while (1) {
        getLineInput(buf, sizeof(buf));
        if (strcmp(buf,"END")==0) break;
        fprintf(fp, "%s\n", buf);
    }
    fclose(fp);
    printf("Timetable saved.\n");
}

void viewTimetable() {
    char line[500]; FILE *fp = fopen("timetable.txt","r");
    if (!fp) { printf("No timetable set.\n"); return; }
    printf("\n--- Timetable ---\n");
    while (fgets(line,sizeof(line),fp)) {
        printf("%s", line);
    }
    fclose(fp);
}


void adminMenu() {
    int ch;
    while (1) {
        printf("\n=== ADMIN MENU ===\n");
        printf("1. Student Management (add/view/search/update/delete)\n");
        printf("2. Attendance (view all)\n");
        printf("3. Marks (view all / add/update)\n");
        printf("4. Fee Management (view all / add/update)\n");
        printf("5. Notice Board (add/view/delete)\n");
        printf("6. Timetable (add/update/view)\n");
        printf("7. Add User (staff/student)\n");
        printf("8. View all users\n");
        printf("9. Change password for any user\n");
        printf("10. Logout\n");
        printf("Enter choice: "); scanf("%d", &ch);
        if (ch==1) {
            int c; printf("\nStudent Mgmt: 1-add 2-viewAll 3-search 4-update 5-delete : "); scanf("%d",&c);
            if (c==1) addStudentRecord();
            else if (c==2) viewAllStudents();
            else if (c==3) searchStudentByRoll();
            else if (c==4) updateStudentByRoll();
            else if (c==5) deleteStudentByRoll();
            else printf("Invalid\n");
        } else if (ch==2) {
            viewAllAttendance();
        } else if (ch==3) {
            int c; printf("\nMarks: 1-add/update 2-viewAll 3-viewByRoll: "); scanf("%d",&c);
            if (c==1) addOrUpdateMarks();
            else if (c==2) viewAllMarks();
            else if (c==3) viewMarksForRollInteractive();
        } else if (ch==4) {
            int c; printf("\nFees: 1-add/update 2-viewAll 3-viewByRoll: "); scanf("%d",&c);
            if (c==1) addOrUpdateFeeRecord();
            else if (c==2) viewAllFees();
            else if (c==3) viewFeeForRollInteractive();
        } else if (ch==5) {
            int c; printf("\nNotices: 1-add 2-view 3-delete: "); scanf("%d",&c);
            if (c==1) addNotice();
            else if (c==2) viewNotices();
            else if (c==3) deleteNoticeByIndex();
        } else if (ch==6) {
            int c; printf("\nTimetable: 1-add/update 2-view: "); scanf("%d",&c);
            if (c==1) addOrUpdateTimetable();
            else if (c==2) viewTimetable();
        } else if (ch==7) addNewUser();
        else if (ch==8) viewAllUsers();
        else if (ch==9) {
            char uname[50], np[50];
            printf("Enter username: "); scanf("%s", uname);
            printf("Enter new password: "); scanf("%s", np);
            if (changePasswordFile(uname, np)) printf("Password changed.\n"); else printf("User not found.\n");
        } else if (ch==10) return;
        else printf("Invalid.\n");
    }
}

void staffMenu() {
    int ch;
    while (1) {
        printf("\n=== STAFF MENU ===\n");
        printf("1. Mark Attendance (present)\n");
        printf("2. Mark Attendance (absent)\n");
        printf("3. View Attendance\n");
        printf("4. Add/Update Marks\n");
        printf("5. Update Marks (view all)\n");
        printf("6. View Students\n");
        printf("7. View Notices\n");
        printf("8. Logout\n");
        printf("Enter choice: "); scanf("%d",&ch);
        if (ch==1) markAttendanceForRoll();
        else if (ch==2) markAbsentForRoll();
        else if (ch==3) viewAllAttendance();
        else if (ch==4) addOrUpdateMarks();
        else if (ch==5) viewAllMarks();
        else if (ch==6) viewAllStudents();
        else if (ch==7) viewNotices();
        else if (ch==8) return;
        else printf("Invalid.\n");
    }
}

void studentMenu() {
    int ch;
    while (1) {
        printf("\n=== STUDENT MENU (%s) ===\n", currentUsername);
        printf("1. View My Marks\n");
        printf("2. View My Attendance\n");
        printf("3. View My Fees\n");
        printf("4. View Notices\n");
        printf("5. View Timetable\n");
        printf("6. Change My Password\n");
        printf("7. Logout\n");
        printf("Enter choice: "); scanf("%d",&ch);
        if (ch==1) {
            if (currentStudentRoll != -1) {
                printf("Showing marks for roll %d\n", currentStudentRoll);
                MarksRec m; FILE *fp = fopen("marks.dat","rb"); int found=0;
                if (!fp) { printf("No marks.\n"); }
                else {
                    while (fread(&m,sizeof(MarksRec),1,fp)) {
                        if (m.roll==currentStudentRoll) {
                            printf("Marks: %d %d %d %d %d | %%: %.2f | %s\n",
                                   m.sub1,m.sub2,m.sub3,m.sub4,m.sub5,m.percent,
                                   (m.sub1<40||m.sub2<40||m.sub3<40||m.sub4<40||m.sub5<40) ? "FAIL" : "PASS");
                            found=1; break;
                        }
                    }
                    if (!found) printf("No marks record.\n");
                    fclose(fp);
                }
            } else {
                printf("No linked roll in users.txt. Input roll: ");
                viewMarksForRollInteractive();
            }
        } else if (ch==2) {
            if (currentStudentRoll != -1) {
                Attendance a; FILE *fp = fopen("attendance.dat","rb"); int found=0;
                if (!fp) { printf("No attendance data.\n"); }
                else {
                    while (fread(&a,sizeof(Attendance),1,fp)) {
                        if (a.roll==currentStudentRoll) {
                            printf("Attendance %d/%d (%.2f%%)\n", a.presentDays, a.totalDays,
                                   a.totalDays ? (100.0f * a.presentDays / a.totalDays) : 0.0f);
                            found=1; break;
                        }
                    }
                    if (!found) printf("No attendance record.\n");
                    fclose(fp);
                }
            } else { printf("No linked roll. Enter roll: "); viewAttendanceByRollInteractive(); }
        } else if (ch==3) {
            if (currentStudentRoll != -1) viewFeeForRollInteractive();
            else { printf("No linked roll. Enter roll: "); viewFeeForRollInteractive(); }
        } else if (ch==4) viewNotices();
        else if (ch==5) viewTimetable();
        else if (ch==6) {
            char np[50]; printf("Enter new password: "); scanf("%s", np);
            if (changePasswordFile(currentUsername, np)) printf("Password changed.\n"); else printf("Failed.\n");
        } else if (ch==7) return;
        else printf("Invalid.\n");
    }
}

void userMenu() {
    int ch;
    while (1) {
        printf("\n=== USER MENU ===\n");
        printf("1. View Students\n");
        printf("2. View Notices\n");
        printf("3. View Timetable\n");
        printf("4. Logout\n");
        printf("Enter choice: "); scanf("%d",&ch);
        if (ch==1) viewAllStudents();
        else if (ch==2) viewNotices();
        else if (ch==3) viewTimetable();
        else if (ch==4) return;
        else printf("Invalid.\n");
    }
}

int main() {
    printf("Student Record Management System (Multi-role)\n");
    if (!fileExists("users.txt")) {
        FILE *fp = fopen("users.txt","w");
        if (fp) {
            fprintf(fp, "admin admin123 ADMIN\n");
            fclose(fp);
            printf("Created default users.txt with admin/admin123\n");
        }
    }
    while (1) {
        int res = login();
        if (res == 1) {
            printf("\nLogin success: %s (%s) linkedRoll=%d\n", currentUsername, currentRole, currentStudentRoll);
            if (strcmp(currentRole,"ADMIN")==0) adminMenu();
            else if (strcmp(currentRole,"STAFF")==0) staffMenu();
            else if (strcmp(currentRole,"STUDENT")==0) studentMenu();
            else userMenu();
            printf("Logged out.\n");
        } else if (res == -1) {
            printf("Wrong password!\n");
        } else if (res == -2) {
            printf("Username does not exist!\n");
        } else {
            printf("Login error or default admin creation required.\n");
        }
        printf("\nExit program? (y/n): ");
        char c; 
		scanf(" %c",&c);
        if (c=='y' || c=='Y') break;
    }
    printf("Goodbye.\n");
    return 0;
}
