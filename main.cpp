#include <iostream>
#include <string>
using namespace std;

// Struct untuk lagu, digunakan di semua struktur data
struct Song {
    int id;
    string title;
    string artist;
    string genre;
    int duration; // dalam detik
    int rating; // 1-5

    // Single Linked List (library)
    Song* next;

    // Double Linked List (playlist)
    Song* prev;
    Song* dnext;

    // Circular Linked List (rotation mode)
    Song* cnext;
};

// Node untuk Priority Queue (Up Next antrian)
struct QueueNode {
    Song* song;
    int priority; // berdasarkan rating
    QueueNode* next;
};

// Node untuk Stack (riwayat putar)
struct StackNode {
    Song* song;
    StackNode* next;
};

// Namespace
namespace MusicPlayer {

    // GLobal pointers untuk tiap struktur data
    Song* libraryHead = nullptr; // Single Linked List head
    Song* playlistHead = nullptr; // Double Linked List head
    Song* playlistTail = nullptr;
    Song* circularHead = nullptr; // Circular Linked List head
    Song* currentCircular = nullptr;
    QueueNode* queueHead = nullptr; // Priority Queue head
    StackNode* stackTop = nullptr; // Stack top
    int songIdCounter = 1;

    // Inline function: cetak separator
    inline void printLine() {
        cout << "--------------------------------------------" << endl;
    }

    // Inline function: cetak detail satu lagu
    inline void printSong(Song* s) {
        cout << "[" << s->id << "] "
              << s->title << " - " << s->artist
              << " | " << s->genre
              << " | " << s->duration / 60 << ":" 
              << (s->duration % 60 < 10 ? "0" : "") << s->duration % 60 // mengubah detik ke format menit:detik
              << " | Rating: " << s->rating << "/5" << endl;
    }

    // Callback: traverse library dan jalankan fungsi action pada tiap node
    void traverseLibrary(Song* head, void (*action)(Song*)) {
        Song* curr = head;
        while (curr != nullptr) {
            action(curr);
            curr = curr->next;
        }
    }

    // Callback action: print lagu
    void actionPrint(Song* s) {
        printSong(s);
    }

    // Function Overloading: searchSong
    // Cari berdasarkan title (substring, case-insensitive)
    int searchSong(const string& keyword, void (*action)(Song*)) {
        int found = 0;
        string kw = keyword;
        for (char& c : kw) c = tolower(c);
        Song* curr = libraryHead;
        while (curr != nullptr) {
            string t = curr->title;
            for (char& c : t) c = tolower(c);
            if (t.find(kw) != string::npos) {
                action(curr);
                found++;
            }
            curr = curr->next;
        }
        return found;
    }

    // Cari berdasarkan artist (substring, case-insensitive)
    int searchSong(const string& keyword, bool byArtist, void (*action)(Song*)) {
        int found = 0;
        string kw = keyword;
        for (char& c : kw) c = tolower(c);
        Song* curr = libraryHead;
        while (curr != nullptr) {
            string a = curr->artist;
            for (char& c : a) c = tolower(c);
            if (a.find(kw) != string::npos) {
                action(curr);
                found++;
            }
            curr = curr->next;
        }
        return found;
    }

    // Cari berdasarkan id
    Song* searchSong(int id) {
        Song* curr = libraryHead;
        while (curr != nullptr) {
            if (curr->id == id) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    // Single Linked List: Library
    // Default argument: rating default = 3
    void addToLibrary(const string& title, const string& artist,
                      const string& genre, int duration, int rating = 3) {
        Song* newSong = new Song();
        newSong->id = songIdCounter++;
        newSong->title = title;
        newSong->artist = artist;
        newSong->genre = genre;
        newSong->duration = duration;
        newSong->rating = rating;
        newSong->next = nullptr;
        newSong->prev = nullptr;
        newSong->dnext = nullptr;
        newSong->cnext = nullptr;

        if (libraryHead == nullptr) {
            libraryHead = newSong;
        } else {
            Song* curr = libraryHead;
            while (curr->next != nullptr) curr = curr->next;
            curr->next = newSong;
        }
        cout << "Lagu \"" << title << "\" berhasil ditambahkan ke library." << endl;
    }

    // Hapus lagu dari library berdasarkan id
    void deleteFromLibrary(int id) {
        if (libraryHead == nullptr) {
            cout << "Library kosong." << endl;
            return;
        }
        Song* curr = libraryHead;
        Song* prev = nullptr;
        while (curr != nullptr && curr->id != id) {
            prev = curr;
            curr = curr->next;
        }
        if (curr == nullptr) {
            cout << "Lagu ID " << id << " tidak ditemukan." << endl;
            return;
        }
        if (prev == nullptr) libraryHead = curr->next;
        else prev->next = curr->next;
        delete curr;
        cout << "Lagu ID " << id << " dihapus dari library." << endl;
    }

    // Tampilkan semua lagu di library
    void displayLibrary() {
        if (libraryHead == nullptr) {
            cout << "Library kosong." << endl;
            return;
        }
        cout << "\n=== LIBRARY LAGU ===" << endl;
        traverseLibrary(libraryHead, actionPrint); // menggunakan callback
        printLine();
    }

    // Double Linked List: Playlist
    // menambahkan lagu ke playlist berdasarkan id dari library
    void addToPlaylist(int songId) {
        Song* original = searchSong(songId);
        if (original == nullptr) {
            cout << "Lagu ID " << songId << " tidak ada di library." << endl;
            return;
        }

        // Buat node baru untuk playlist (copy data, pointer terpisah)
        Song* node = new Song();
        *node = *original;
        node->prev = nullptr;
        node->dnext = nullptr;
        node->next = nullptr;
        node->cnext = nullptr;

        if (playlistHead == nullptr) {
            playlistHead = node;
            playlistTail = node;
        } else {
            node->prev = playlistTail;
            playlistTail->dnext = node;
            playlistTail = node;
        }
        cout << "\"" << original->title << "\" ditambahkan ke playlist." << endl;
    }

    // hapus lagu dari playlist berdasarkan id
    void deleteFromPlaylist(int songId) {
        Song* curr = playlistHead;
        while (curr != nullptr && curr->id != songId) curr = curr->dnext;
        if (curr == nullptr) {
            cout << "Lagu ID " << songId << " tidak ada di playlist." << endl;
            return;
        }
        if (curr->prev  != nullptr) curr->prev->dnext = curr->dnext;
        else playlistHead = curr->dnext;
        if (curr->dnext != nullptr) curr->dnext->prev = curr->prev;
        else playlistTail = curr->prev;
        cout << "\"" << curr->title << "\" dihapus dari playlist." << endl;
        delete curr;
    }

    // tampilkan semua lagu di playlist
    void displayPlaylist() {
        if (playlistHead == nullptr) {
            cout << "Playlist kosong." << endl;
            return;
        }
        cout << "\n=== PLAYLIST ===" << endl;
        Song* curr = playlistHead;
        int idx = 1;
        while (curr != nullptr) {
            cout << idx++ << ". ";
            printSong(curr);
            curr = curr->dnext;
        }
        printLine();
    }

    // Stack: push lagu ke riwayat
    void pushHistory(Song* s) {
        StackNode* node = new StackNode();
        node->song = s;
        node->next = stackTop;
        stackTop  = node;
    }

    // Stack: pop lagu dari riwayat
    void popHistory() {
        if (stackTop == nullptr) return;
        StackNode* tmp = stackTop;
        cout << "Lagu yang di-pop dari riwayat: ";
        printSong(tmp->song);
        stackTop = stackTop->next;
        delete tmp;
    }

    // Navigasi playlist: next
    void playlistNext(Song*& current) {
        if (current == nullptr || current->dnext == nullptr) {
            cout << "Sudah di lagu terakhir playlist." << endl;
            return;
        }
        current = current->dnext;
        // pushHistory(current); // catat ke riwayat
        cout << ">> Sekarang: "; printSong(current);
    }

    // Navigasi playlist: prev
    void playlistPrev(Song*& current) {
        if (current == nullptr || current->prev == nullptr) {
            cout << "Sudah di lagu pertama playlist." << endl;
            return;
        }
        current = current->prev;
        // pushHistory(current); // catat ke riwayat
        cout << "<< Kembali ke: "; printSong(current);
    }

    // Circular Linked List: Repeat Mode
    void buildCircularFromPlaylist() {
        // Reset circular list dulu
        circularHead = nullptr;
        currentCircular = nullptr;

        if (playlistHead == nullptr) {
            cout << "Playlist kosong, tidak bisa build circular." << endl;
            return;
        }

        // Salin lagu dari playlist ke circular linked list
        Song* tail = nullptr;
        Song* src = playlistHead;
        while (src != nullptr) {
            Song* node  = new Song();
            *node = *src;
            node->cnext = nullptr;  

            if (circularHead == nullptr) {
                circularHead = node;
                tail = node;
            } else {
                tail->cnext = node;
                tail = node;
            }
            src = src->dnext;
        }
        tail->cnext = circularHead; // sambungkan ujung ke head -> circular
        currentCircular = circularHead;
        cout << "Circular mode aktif. Total lagu: ";
        int count = 0;
        Song* tmp = circularHead;
        do { count++; tmp = tmp->cnext; } while (tmp != circularHead);
        cout << count << " lagu." << endl;
    }

    // Putar lagu berikutnya di circular mode
    void playNextCircular() {
        if (currentCircular == nullptr) {
            cout << "Circular mode belum aktif. Bangun dari playlist dulu." << endl;
            return;
        }
        cout << "[REPEAT] Memutar: "; printSong(currentCircular);
        currentCircular = currentCircular->cnext;
        pushHistory(currentCircular); // catat ke riwayat
    }

    // Priority Queue: Up Next berdasarkan tingginya rating
    void enqueue(int songId) {
        Song* s = searchSong(songId);
        if (s == nullptr) {
            cout << "Lagu ID " << songId << " tidak ditemukan." << endl;
            return;
        }

        QueueNode* newNode = new QueueNode();
        newNode->song = s;
        newNode->priority = s->rating;
        newNode->next = nullptr;

        // Insert sesuai priority (descending)
        if (queueHead == nullptr || newNode->priority > queueHead->priority) {
            newNode->next = queueHead;
            queueHead = newNode;
        } else {
            QueueNode* curr = queueHead;
            while (curr->next != nullptr && curr->next->priority >= newNode->priority) {
                curr = curr->next;
            }
            newNode->next = curr->next;
            curr->next = newNode;
        }
        cout << "\"" << s->title << "\" (rating " << s->rating << ") masuk antrian." << endl;
    }

    // Dequeue: putar lagu dengan priority tertinggi
    void dequeue() {
        if (queueHead == nullptr) {
            cout << "Antrian kosong." << endl;
            return;
        }
        QueueNode* front = queueHead;
        queueHead = queueHead->next;

        pushHistory(front->song); // catat ke riwayat
        cout << "[PUTAR] " ; printSong(front->song);
        delete front;
    }

    // Tampilkan antrian saat ini
    void displayQueue() {
        if (queueHead == nullptr) {
            cout << "Antrian kosong." << endl;
            return;
        }
        cout << "\n=== ANTRIAN (Priority by Rating) ===" << endl;
        QueueNode* curr = queueHead;
        int idx = 1;
        while (curr != nullptr) {
            cout << idx++ << ". [Priority " << curr->priority << "] ";
            printSong(curr->song);
            curr = curr->next;
        }
        printLine();
    }

    // Stack: History putar lagu
    void displayHistory() {
        if (stackTop == nullptr) {
            cout << "Riwayat kosong." << endl;
            return;
        }
        cout << "\n=== RIWAYAT PUTAR (terbaru di atas) ===" << endl;
        StackNode* curr = stackTop;
        int idx = 1;
        while (curr != nullptr) {
            cout << idx++ << ". "; printSong(curr->song);
            curr = curr->next;
        }
        printLine();
    }

    // CLEANUP: bebaskan semua memori
    void cleanup() {
        // Library
        Song* curr = libraryHead;
        while (curr != nullptr) {
            Song* tmp = curr->next;
            delete curr;
            curr = tmp;
        }
        // Playlist
        Song* pc = playlistHead;
        while (pc != nullptr) {
            Song* tmp = pc->dnext;
            delete pc;
            pc = tmp;
        }
        // Circular (harus putus loop dulu)
        if (circularHead != nullptr) {
            Song* cc = circularHead;
            Song* tail = circularHead;
            while (tail->cnext != circularHead) tail = tail->cnext;
            tail->cnext = nullptr; // putus circular
            while (cc != nullptr) {
                Song* tmp = cc->cnext;
                delete cc;
                cc = tmp;
            }
        }
        // Queue
        QueueNode* qc = queueHead;
        while (qc != nullptr) {
            QueueNode* tmp = qc->next;
            delete qc;
            qc = tmp;
        }
        // Stack
        StackNode* sc = stackTop;
        while (sc != nullptr) {
            StackNode* tmp = sc->next;
            delete sc;
            sc = tmp;
        }
    }

} // end namespace MusicPlayer

// Helper function untuk input string 
string inputLine(const string& prompt) {
    cout << prompt;
    string val;
    getline(cin, val);
    return val;
}

// MAIN
int main() {
    // Data Dummy
    MusicPlayer::addToLibrary("Bohemian Rhapsody", "Queen", "Rock", 354, 5);
    MusicPlayer::addToLibrary("Blinding Lights", "The Weeknd", "Pop", 200, 4);
    MusicPlayer::addToLibrary("Shape of You", "Ed Sheeran", "Pop", 234, 4);
    MusicPlayer::addToLibrary("Stairway to Heaven","Led Zeppelin","Rock", 482, 5);
    MusicPlayer::addToLibrary("Levitating", "Dua Lipa", "Pop", 203, 3);
    MusicPlayer::addToLibrary("Smells Like Teen", "Nirvana", "Grunge", 301, 4);

    // currentSong untuk navigasi Double Linked List playlist
    Song* currentSong = nullptr;

    int choice;
    do {
        cout << "\n============================================" << endl;
        cout << "   SISTEM MANAJEMEN PLAYLIST & MUSIK" << endl;
        cout << "============================================" << endl;
        cout << " 1. Tambah lagu ke library" << endl;
        cout << " 2. Hapus lagu dari library" << endl;
        cout << " 3. Tampilkan library" << endl;
        cout << " 4. Cari lagu (by title / artist)" << endl;
        cout << " 5. Tambah lagu ke playlist" << endl;
        cout << " 6. Hapus lagu dari playlist" << endl;
        cout << " 7. Tampilkan playlist + navigasi prev/next" << endl;
        cout << " 8. Tambah lagu ke antrian (Enqueue)" << endl;
        cout << " 9. Putar dari antrian (Dequeue)" << endl;
        cout << "10. Aktifkan repeat mode + putar next (Circular)" << endl;
        cout << "11. Tampilkan riwayat putar (Stack)" << endl;
        cout << "12. Hapus lagu dari riwayat (Pop Stack)" << endl;
        cout << " 0. Exit" << endl;
        cout << "Pilih: "; cin >> choice;

        switch (choice) {

            // 1. Tambah lagu ke library (Single Linked List)
            case 1: {
                cin.ignore();
                string title = inputLine("Title  : ");
                string artist = inputLine("Artist : ");
                string genre = inputLine("Genre  : ");
                int dur, rat;
                cout << "Durasi (detik): "; cin >> dur;
                cout << "Rating (1-5)  : "; cin >> rat;
                MusicPlayer::addToLibrary(title, artist, genre, dur, rat);
                break;
            }

            // 2. Hapus lagu dari library
            case 2: {
                MusicPlayer::displayLibrary();
                int id; cout << "ID lagu yang dihapus: "; cin >> id;
                MusicPlayer::deleteFromLibrary(id);
                break;
            }

            // 3. Tampilkan library
            case 3:
                MusicPlayer::displayLibrary();
                break;

            // 4. Cari lagu (overloading: by title atau artist, substring)
            case 4: {
                cout << "Cari by: 1. Title  2. Artist -> ";
                int opt; cin >> opt;
                cin.ignore();
                if (opt == 1) {
                    string t = inputLine("Keyword title  : ");
                    int n = MusicPlayer::searchSong(t, MusicPlayer::actionPrint);
                    if (n == 0) cout << "Tidak ditemukan." << endl;
                    else cout << n << " lagu ditemukan." << endl;
                } else if (opt == 2) {
                    string a = inputLine("Keyword artist : ");
                    int n = MusicPlayer::searchSong(a, true, MusicPlayer::actionPrint);
                    if (n == 0) cout << "Tidak ditemukan." << endl;
                    else cout << n << " lagu ditemukan." << endl;
                } else {
                    cout << "Pilihan tidak valid." << endl;
                }
                break;
            }

            // 5. Tambah lagu ke playlist (Double Linked List)
            case 5: {
                MusicPlayer::displayLibrary();
                int id; cout << "ID lagu: "; cin >> id;
                MusicPlayer::addToPlaylist(id);
                if (currentSong == nullptr) currentSong = MusicPlayer::playlistHead;
                break;
            }

            // 6. Hapus lagu dari playlist
            case 6: {
                MusicPlayer::displayPlaylist();
                if (MusicPlayer::playlistHead == nullptr) break;
                int id; cout << "ID lagu yang dihapus: "; cin >> id;
                MusicPlayer::deleteFromPlaylist(id);
                break;
            }

            // 7. Tampilkan playlist + navigasi prev/next
            case 7: {
                MusicPlayer::displayPlaylist();
                if (MusicPlayer::playlistHead == nullptr) break;
                if (currentSong == nullptr) currentSong = MusicPlayer::playlistHead;
                cout << "Sekarang di: "; MusicPlayer::printSong(currentSong);
                cout << "Navigasi: 1. Next  2. Prev  0. Batal -> ";
                int nav; cin >> nav;
                switch (nav) {
                    case 1: MusicPlayer::playlistNext(currentSong); break;
                    case 2: MusicPlayer::playlistPrev(currentSong); break;
                    default: break;
                } 
                break;
            }

            // 8. Tambah lagu ke antrian (Enqueue)
            case 8: {
                MusicPlayer::displayLibrary();
                int id; cout << "ID lagu yang ingin di-queue: "; cin >> id;
                MusicPlayer::enqueue(id);
                MusicPlayer::displayQueue();
                break;
            }

            // 9. Putar dari antrian (Dequeue)
            case 9:
                MusicPlayer::dequeue();
                MusicPlayer::displayQueue();
                break;

            // 10. Aktifkan repeat mode (Circular) + putar next
            case 10: {
                if (MusicPlayer::circularHead == nullptr) {
                    MusicPlayer::buildCircularFromPlaylist();
                }
                MusicPlayer::playNextCircular();
                break;
            }

            // 11. Tampilkan riwayat putar (Stack)
            case 11:
                MusicPlayer::displayHistory();
                break;

            case 12:
                MusicPlayer::displayHistory();
                if (MusicPlayer::stackTop == nullptr) {
                    cout << "Riwayat kosong, tidak bisa pop." << endl;
                    break;
                }
                MusicPlayer::popHistory();
                break;

                // MusicPlayer::displayPlaylist();
                // if (MusicPlayer::playlistHead == nullptr) break;
                // int id; cout << "ID lagu yang dihapus: "; cin >> id;
                // MusicPlayer::deleteFromPlaylist(id);
                // break;

            case 0:
                cout << "Sampai jumpa!" << endl;
                break;

            default:
                cout << "Pilihan tidak valid." << endl;
        }

    } while (choice != 0);

    MusicPlayer::cleanup();
    return 0;
}
