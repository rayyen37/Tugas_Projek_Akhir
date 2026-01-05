#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <random> 
#include <fstream>
#include <iomanip>

using namespace std;

// --- KONSTANTA VISUAL ---
const char DINDING = char(219);
const char JALAN = ' ';
const char PLAYER = 'P';
const char MONSTER = 'M';
const char FINISH = 'F';

// --- KONFIGURASI SKILL ---
const int DURASI_SKILL = 4000;  // 4 detik
const int COOLDOWN_SKILL = 50000; // 50 detik

struct Titik {
    int x, y;
};

// --- STRUKTUR DATA LEADERBOARD ---
struct HighScore {
    string nama;
    int totalDetik;
    string waktuStr; // untuk tampilan "X menit Y detik"
    int difficulty;
};

// Fungsi Helper Posisi Kursor
void gotoXY(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Fungsi Sorting untuk Leaderboard (Tercepat ke Terlambat)
bool compareHighScore(const HighScore& a, const HighScore& b) {
    return a.totalDetik < b.totalDetik;
}

// --- FUNGSI BARU: MENAMPILKAN LEADERBOARD ---
void tampilkanLeaderboard() {
    ifstream file("waktu.txt");
    if (!file.is_open()) {
        cout << "\n[INFO] Belum ada data leaderboard (file waktu.txt tidak ditemukan).\n";
        return;
    }

    vector<HighScore> daftarSkor;
    string baris;
    HighScore tempSkor;
    bool dataReady = false;

    // Parsing file txt
    while (getline(file, baris)) {
        if (baris.find("PLAYER  : ") != string::npos) {
            tempSkor.nama = baris.substr(10); // Ambil teks setelah "PLAYER  : "
        }
        else if (baris.find("WAKTU   : ") != string::npos) {
            tempSkor.waktuStr = baris.substr(10);
            int m = 0, s = 0;
            sscanf(baris.c_str(), "WAKTU   : %d menit %d detik", &m, &s);
            tempSkor.totalDetik = (m * 60) + s;
        }
        else if (baris.find("DIFF    : ") != string::npos) {
            tempSkor.difficulty = stoi(baris.substr(10));
            dataReady = true; // Data penting sudah lengkap
        }
        else if (baris.find("-----------------------------") != string::npos) {
            if (dataReady) {
                daftarSkor.push_back(tempSkor);
                dataReady = false; // Reset untuk pemain berikutnya
            }
        }
    }
    file.close();

    // Sorting data
    sort(daftarSkor.begin(), daftarSkor.end(), compareHighScore);

    // Tampilkan Tabel
    cout << "\n========== TOP 5 LEADERBOARD ==========\n";
    cout << left << setw(4) << "No"
        << setw(15) << "Nama"
        << setw(20) << "Waktu"
        << setw(10) << "Diff" << endl;
    cout << "---------------------------------------\n";

    int limit = min((int)daftarSkor.size(), 5); // Tampilkan max 5
    for (int i = 0; i < limit; i++) {
        cout << left << setw(4) << (i + 1)
            << setw(15) << daftarSkor[i].nama
            << setw(20) << daftarSkor[i].waktuStr
            << setw(10) << daftarSkor[i].difficulty << endl;
    }
    cout << "=======================================\n";
}

void tampilkanPeta(const vector<string>& peta) {
    string buffer = "";
    for (const string& baris : peta) {
        buffer += baris + "\n";
    }
    cout << buffer;
}

// fungsi loading


void showConsoleCursor(bool showFlag) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}


void animasiLoading(string teks, int durasiDetik) {
    system("cls");
    showConsoleCursor(false); // menyembunyikan kursor

    int barWidth = 40;
    int updateRate = 50;
    int totalSteps = (durasiDetik * 1000) / updateRate;

    int centerX = 20;
    int centerY = 15;

    for (int i = 0; i <= totalSteps; i++) {
        // hitung waktu sisa
        float progress = (float)i / totalSteps;
        int filledLength = (int)(progress * barWidth);
        int waktuSisa = durasiDetik - (i * updateRate / 1000);
        if (waktuSisa < 0) waktuSisa = 0;

        // Gambar Border Atas
        gotoXY(centerX, centerY - 1);
        cout << char(218);
        for (int j = 0; j < barWidth + 25; j++) cout << char(196); // Lebarkan dikit border
        cout << char(191);

        // Gambar Tengah (Teks + Bar)
        gotoXY(centerX, centerY);
        cout << char(179) << " " << left << setw(20) << teks; // Teks rata kiri, lebar 20 char

        for (int j = 0; j < filledLength; j++) cout << char(219);
        for (int j = filledLength; j < barWidth - 15; j++) cout << char(176);

        cout << " " << waktuSisa << "s " << char(179);

        // Gambar Border Bawah
        gotoXY(centerX, centerY + 1);
        cout << char(192);
        for (int j = 0; j < barWidth + 25; j++) cout << char(196);
        cout << char(217);

        Sleep(updateRate);
    }

    showConsoleCursor(true);
    Sleep(300); // Jeda transisi
}

void tampilkanStatusSkill(string nama, bool aktif, clock_t readyTime, clock_t now) {
    cout << nama << ": ";
    if (aktif) {
        cout << "[AKTIF] Sisa: " << (readyTime - now - (COOLDOWN_SKILL - DURASI_SKILL)) / 1000 << "d   ";
    }
    else if (now < readyTime) {
        cout << "[CD] : " << (readyTime - now) / 1000 << "d   ";
    }
    else {
        cout << "[SIAP] Tekan tombol!   ";
    }
    cout << endl;
}

std::mt19937 rng(std::random_device{}());

int main() {
    system("mode con cols=80 lines=40");
    srand(time(0));
    char pilihanMain;
    string namaPlayer;

    animasiLoading("WELCOME TO MAZE!", 3); 
    animasiLoading("ENJOY THE GAME!", 3);  

    do {
        system("cls");
        cout << "==========================\n";
        cout << "Masukkan Nama Player: ";
        getline(cin, namaPlayer);

        
        tampilkanLeaderboard();
        cout << "\nTekan ENTER untuk lanjut ke pemilihan level...";
        cin.get();
        

        int tinggi, lebar, difficulty;
        system("cls");
        cout << "=== MAZE GAME - " << namaPlayer << " ===\n";
        cout << "1. Easy (9x9)\n2. Normal (18x18)\n3. Hard (36x36)\nPilih Difficulty: ";
        cin >> difficulty;
        cin.ignore(); // handle newline buffer

        animasiLoading("PREPARING MAP...", 2); // animasi loading sebelum masuk maze

        // Setup ukuran map
        if (difficulty == 1) { lebar = 9; tinggi = 9; }
        else if (difficulty == 2) { lebar = 18; tinggi = 18; }
        else { difficulty = 3; lebar = 30; tinggi = 30; }

        if (tinggi % 2 == 0) tinggi++;
        if (lebar % 2 == 0) lebar++;

        // Inisialisasi Map
        vector<string> baseMap(tinggi, string(lebar, DINDING));
        vector<Titik> stack;

        int x = 1, y = 1;
        baseMap[y][x] = JALAN;
        stack.push_back({ x, y });

        // Generator Maze (DFS Algorithm)
        while (!stack.empty()) {
            Titik cur = stack.back();
            vector<pair<int, int>> dir = { {0,-2},{0,2},{-2,0},{2,0} };
            shuffle(dir.begin(), dir.end(), rng);

            bool found = false;
            for (auto d : dir) {
                int nx = cur.x + d.first;
                int ny = cur.y + d.second;

                if (nx > 0 && nx < lebar - 1 && ny > 0 && ny < tinggi - 1 && baseMap[ny][nx] == DINDING) {
                    baseMap[cur.y + d.second / 2][cur.x + d.first / 2] = JALAN;
                    baseMap[ny][nx] = JALAN;
                    stack.push_back({ nx, ny });
                    found = true;
                    break;
                }
            }
            if (!found) stack.pop_back();
        }

        Titik finish = { lebar - 2, tinggi - 2 };
        baseMap[finish.y][finish.x] = JALAN;

        // Setup Monster
        vector<Titik> monsters;
        int monsterTickThreshold = (difficulty == 1) ? 9999 : (difficulty == 2 ? 5 : 2);
        if (difficulty >= 2) {
            int jumlahMonster = (difficulty == 2) ? 3 : 8;
            for (int i = 0; i < jumlahMonster; i++) {
                int mx, my;
                do {
                    mx = rand() % lebar;
                    my = rand() % tinggi;
                } while (baseMap[my][mx] != JALAN || (mx < 5 && my < 5));
                monsters.push_back({ mx, my });
            }
        }

        // Variabel Game Loop
        bool isFrozen = false, isInvisible = false;
        clock_t freezeReady = 0, invisibleReady = 0;
        clock_t freezeEnd = 0, invisibleEnd = 0;
        bool isGameOver = false;
        int tick = 0;

        clock_t gameStartTime = clock();
        system("cls");

        // --- GAME LOOP ---
        while (true) {
            clock_t now = clock();

            // Cek durasi skill habis
            if (isFrozen && now > freezeEnd) isFrozen = false;
            if (isInvisible && now > invisibleEnd) isInvisible = false;

            // Render Buffer
            vector<string> renderMap = baseMap;
            renderMap[finish.y][finish.x] = FINISH;
            renderMap[y][x] = isInvisible ? 'I' : PLAYER;
            for (auto& m : monsters) renderMap[m.y][m.x] = isFrozen ? '*' : MONSTER;

            gotoXY(0, 0);
            tampilkanPeta(renderMap);

            // Info HUD
            cout << "\n=== Skill Stats ===" << endl;
            tampilkanStatusSkill("1. Freeze", isFrozen, freezeReady, now);
            tampilkanStatusSkill("2. Invisible", isInvisible, invisibleReady, now);

            // Timer Real-time
            int elapsed = (now - gameStartTime) / CLOCKS_PER_SEC;
            cout << "Waktu: " << elapsed / 60 << "m " << elapsed % 60 << "s | Diff: " << difficulty << "   \n";

            // Input Player
            if (_kbhit()) {
                int key = _getch();
                if (key == 224) key = _getch();
                int nx = x, ny = y;

                if (key == 'w' || key == 72) ny--;
                if (key == 's' || key == 80) ny++;
                if (key == 'a' || key == 75) nx--;
                if (key == 'd' || key == 77) nx++;

                // Skill Activation
                if (key == '1' && now >= freezeReady) {
                    isFrozen = true;
                    freezeEnd = now + DURASI_SKILL;
                    freezeReady = now + COOLDOWN_SKILL;
                }
                if (key == '2' && now >= invisibleReady) {
                    isInvisible = true;
                    invisibleEnd = now + DURASI_SKILL;
                    invisibleReady = now + COOLDOWN_SKILL;
                }

                if (nx >= 0 && nx < lebar && ny >= 0 && ny < tinggi && baseMap[ny][nx] == JALAN) {
                    x = nx; y = ny;
                }
            }

            // --- CEK MENANG ---
            if (x == finish.x && y == finish.y) {
                system("cls");
                int finalTime = (clock() - gameStartTime) / CLOCKS_PER_SEC;

                cout << "==========================\n";
                cout << "      YOU WIN!      \n";
                cout << "==========================\n";
                cout << "Nama   : " << namaPlayer << endl;
                cout << "Waktu  : " << finalTime / 60 << " menit " << finalTime % 60 << " detik\n";

                // Simpan ke waktu.txt
                time_t t = time(0);
                tm* ltm = localtime(&t);
                ofstream file("waktu.txt", ios::app);
                file << "PLAYER  : " << namaPlayer << endl;
                file << "STATUS  : MENANG" << endl;
                file << "WAKTU   : " << finalTime / 60 << " menit " << finalTime % 60 << " detik" << endl;
                file << "DIFF    : " << difficulty << endl;
                file << "TANGGAL : " << ltm->tm_mday << "-" << 1 + ltm->tm_mon << "-" << 1900 + ltm->tm_year << endl;
                file << "-----------------------------\n";
                file.close();

                cout << "\n[Data disimpan ke Leaderboard]\n";
                cout << "Main lagi? (y/n): ";
                cin >> pilihanMain;
                cin.ignore();
                break;
            }

            // --- MONSTER LOGIC ---
            tick++;
            if (!isFrozen && tick % monsterTickThreshold == 0) {
                for (auto& m : monsters) {
                    int dir = rand() % 4;
                    int mx = m.x, my = m.y;
                    if (dir == 0) my--; else if (dir == 1) my++;
                    else if (dir == 2) mx--; else if (dir == 3) mx++;

                    if (mx >= 0 && mx < lebar && my >= 0 && my < tinggi && baseMap[my][mx] == JALAN) {
                        m.x = mx; m.y = my;
                    }
                    if (m.x == x && m.y == y && !isInvisible) {
                        system("cls");
                        cout << "GAME OVER! Dimakan Monster!\nMain lagi? (y/n): ";
                        cin >> pilihanMain;
                        cin.ignore();
                        isGameOver = true;
                        break;
                    }
                }
            }
            if (isGameOver) break;

            // Cek tabrakan player ke monster
            for (auto& m : monsters) {
                if (m.x == x && m.y == y && !isInvisible) {
                    system("cls");
                    cout << "GAME OVER! Menabrak Monster!\nMain lagi? (y/n): ";
                    cin >> pilihanMain;
                    cin.ignore();
                    isGameOver = true;
                    break;
                }
            }
            if (isGameOver) break;

            Sleep(100);
        }

    } while (pilihanMain == 'Y' || pilihanMain == 'y');

    return 0;
}