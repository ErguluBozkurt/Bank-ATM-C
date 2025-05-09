#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

// Sabit tanımlamalar
#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 10
#define PASSWORD_LENGTH 6
#define ACCOUNT_NO_LENGTH 7
#define NAME_LENGTH 20

// İşlem yapısı
typedef struct {
    char date[20];
    char type[30];
    float amount;
    float balance_after;
    char target_account[ACCOUNT_NO_LENGTH];
} Transaction;

// Banka hesabı yapısı
typedef struct {
    char name[NAME_LENGTH];
    char surname[NAME_LENGTH];
    char account_no[ACCOUNT_NO_LENGTH];
    char password[PASSWORD_LENGTH];
    float balance;
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
    int login_attempts;
    bool is_locked;
    char last_login[20];
} BankAccount;

// ATM yapısı
typedef struct {
    BankAccount accounts[MAX_ACCOUNTS];
    int account_count;
    BankAccount *current_account;
} ATM;

// Prototipler
void initialize_atm(ATM *atm);
void show_welcome_screen();
void show_main_menu(ATM *atm);
void view_account_info(BankAccount *account);
bool withdraw(BankAccount *account, float amount);
bool deposit(BankAccount *account, float amount);
bool transfer(ATM *atm, BankAccount *sender, BankAccount *receiver, float amount);
void view_transaction_history(BankAccount *account);
bool change_password(BankAccount *account, const char *new_password);
bool login(BankAccount *account, const char *entered_password);
BankAccount* find_account(ATM *atm, const char *account_no);
void add_transaction(BankAccount *account, const char *type, float amount, const char *target_account);
void get_current_datetime(char *buffer);
void clear_input_buffer();
void delay(unsigned int milliseconds);

// Ana fonksiyon
int main() {
    ATM atm;
    initialize_atm(&atm);
    
    while (1) {
        show_welcome_screen();
        
        char account_no[ACCOUNT_NO_LENGTH];
        printf("Hesap No: ");
        scanf("%6s", account_no);
        clear_input_buffer();
        
        BankAccount *account = find_account(&atm, account_no);
        if (account == NULL) {
            printf("Hesap bulunamadi!\n");
            delay(2000);
            continue;
        }
        
        if (account->is_locked) {
            printf("Bu hesap kilitlenmis. Bankanizla iletisime gecin.\n");
            delay(2000);
            continue;
        }
        
        int attempts = 0;
        bool logged_in = false;
        
        while (attempts < 3) {
            char password[PASSWORD_LENGTH];
            printf("Sifrenizi giriniz: ");
            scanf("%5s", password);
            clear_input_buffer();
            
            if (login(account, password)) {
                logged_in = true;
                break;
            }
            attempts++;
        }
        
        if (!logged_in) {
            continue;
        }
        
        atm.current_account = account;
        show_main_menu(&atm);
    }
    
    return 0;
}

// ATM'yi başlangıç verileriyle başlat
void initialize_atm(ATM *atm) {
    atm->account_count = 3;
    atm->current_account = NULL;
    
    // Örnek hesaplar oluştur
    strcpy(atm->accounts[0].name, "Ahmet");
    strcpy(atm->accounts[0].surname, "Yilmaz");
    strcpy(atm->accounts[0].account_no, "123456");
    strcpy(atm->accounts[0].password, "11111");
    atm->accounts[0].balance = 15000.0f;
    atm->accounts[0].transaction_count = 0;
    atm->accounts[0].login_attempts = 0;
    atm->accounts[0].is_locked = false;
    strcpy(atm->accounts[0].last_login, "");
    
    strcpy(atm->accounts[1].name, "Mehmet");
    strcpy(atm->accounts[1].surname, "Kaya");
    strcpy(atm->accounts[1].account_no, "654321");
    strcpy(atm->accounts[1].password, "22222");
    atm->accounts[1].balance = 25000.0f;
    atm->accounts[1].transaction_count = 0;
    atm->accounts[1].login_attempts = 0;
    atm->accounts[1].is_locked = false;
    strcpy(atm->accounts[1].last_login, "");
    
    strcpy(atm->accounts[2].name, "Ayse");
    strcpy(atm->accounts[2].surname, "Demir");
    strcpy(atm->accounts[2].account_no, "987654");
    strcpy(atm->accounts[2].password, "33333");
    atm->accounts[2].balance = 5000.0f;
    atm->accounts[2].transaction_count = 0;
    atm->accounts[2].login_attempts = 0;
    atm->accounts[2].is_locked = false;
    strcpy(atm->accounts[2].last_login, "");
}

// Hoşgeldin ekranı
void show_welcome_screen() {
    system("clear || cls");
    printf("\n==================================================\n");
    printf("          BANKAMATIK SISTEMINE HOSGELDINIZ        \n");
    printf("==================================================\n");
    printf("Lutfen kartinizi takiniz (hesap numarasi giriniz)\n\n");
}

// Ana menü
void show_main_menu(ATM *atm) {
    int choice;
    
    while (1) {
        system("clear || cls");
        printf("\n==================================================\n");
        printf("                      ANA MENU                   \n");
        printf("==================================================\n");
        printf("Hosgeldiniz, %s %s\n", atm->current_account->name, atm->current_account->surname);
        printf("1 - Hesap Bilgileri\n");
        printf("2 - Para Cekme\n");
        printf("3 - Para Yatirma\n");
        printf("4 - Para Transferi\n");
        printf("5 - Islem Gecmisi\n");
        printf("6 - Sifre Degistirme\n");
        printf("0 - Cikis\n");
        printf("==================================================\n");
        
        printf("Lutfen islem seciniz (0-6): ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                view_account_info(atm->current_account);
                break;
            case 2:
                {
                    float amount;
                    printf("Cekilecek miktari giriniz: ");
                    scanf("%f", &amount);
                    clear_input_buffer();
                    withdraw(atm->current_account, amount);
                }
                break;
            case 3:
                {
                    float amount;
                    printf("Yatirilacak miktari giriniz: ");
                    scanf("%f", &amount);
                    clear_input_buffer();
                    deposit(atm->current_account, amount);
                }
                break;
            case 4:
                {
                    char target_account_no[ACCOUNT_NO_LENGTH];
                    printf("Transfer edilecek hesap numarasini giriniz: ");
                    scanf("%6s", target_account_no);
                    clear_input_buffer();
                    
                    BankAccount *target_account = find_account(atm, target_account_no);
                    if (target_account == NULL) {
                        printf("Hesap bulunamadi!\n");
                        delay(2000);
                        break;
                    }
                    
                    printf("\nHesap Sahibi: %s %s\n", target_account->name, target_account->surname);
                    printf("Transferi onayliyor musunuz? (E/H): ");
                    char confirm;
                    scanf("%c", &confirm);
                    clear_input_buffer();
                    
                    if (confirm != 'E' && confirm != 'e') {
                        printf("Transfer iptal edildi.\n");
                        delay(2000);
                        break;
                    }
                    
                    float amount;
                    printf("\nTransfer miktarini giriniz: ");
                    scanf("%f", &amount);
                    clear_input_buffer();
                    
                    transfer(atm, atm->current_account, target_account, amount);
                }
                break;
            case 5:
                view_transaction_history(atm->current_account);
                break;
            case 6:
                {
                    char old_password[PASSWORD_LENGTH];
                    char new_password[PASSWORD_LENGTH];
                    char confirm_password[PASSWORD_LENGTH];
                    
                    printf("Mevcut sifrenizi giriniz: ");
                    scanf("%5s", old_password);
                    clear_input_buffer();
                    
                    if (strcmp(old_password, atm->current_account->password) != 0) {
                        printf("Mevcut sifreniz yanlis!\n");
                        delay(2000);
                        break;
                    }
                    
                    printf("Yeni sifrenizi giriniz (5 haneli rakam): ");
                    scanf("%5s", new_password);
                    clear_input_buffer();
                    
                    printf("Yeni sifrenizi tekrar giriniz: ");
                    scanf("%5s", confirm_password);
                    clear_input_buffer();
                    
                    if (strcmp(new_password, confirm_password) != 0) {
                        printf("Sifreler uyusmuyor!\n");
                        delay(2000);
                        break;
                    }
                    
                    change_password(atm->current_account, new_password);
                }
                break;
            case 0:
                printf("Cikis yapiliyor... Kartinizi almayi unutmayiniz.\n");
                delay(2000);
                atm->current_account = NULL;
                return;
            default:
                printf("Gecersiz secim! Lutfen 0-6 arasi bir sayi giriniz.\n");
                delay(2000);
        }
    }
}

// Hesap bilgilerini göster
void view_account_info(BankAccount *account) {
    system("clear || cls");
    printf("\n==================================================\n");
    printf("                HESAP BILGILERI                  \n");
    printf("==================================================\n");
    printf("Ad Soyad: %s %s\n", account->name, account->surname);
    printf("Hesap No: %s\n", account->account_no);
    printf("Bakiye: %.2f TL\n", account->balance);
    printf("Son Giris: %s\n", account->last_login);
    printf("==================================================\n\n");
    delay(2000);
}

// Para çekme işlemi
bool withdraw(BankAccount *account, float amount) {
    if (account->is_locked) {
        printf("Hesabiniz gecici olarak kilitlenmistir. Bankanizla iletisime gecin.\n");
        delay(2000);
        return false;
    }
    
    if (amount <= 0) {
        printf("Gecersiz miktar girdiniz.\n");
        delay(2000);
        return false;
    }
    
    if (amount > account->balance) {
        printf("Yetersiz bakiye!\n");
        delay(2000);
        return false;
    }
    
    if (amount > 10000) { // Günlük limit kontrolü
        printf("Gunluk cekim limitiniz 10,000 TL'dir.\n");
        delay(2000);
        return false;
    }
    
    account->balance -= amount;
    add_transaction(account, "Para Cekme", amount, "");
    printf("\n%.2f TL cekiliyor... Lutfen paranizi aliniz.\n", amount);
    printf("Yeni bakiye: %.2f TL\n", account->balance);
    delay(2000);
    return true;
}

// Para yatırma işlemi
bool deposit(BankAccount *account, float amount) {
    if (amount <= 0) {
        printf("Gecersiz miktar girdiniz.\n");
        delay(2000);
        return false;
    }
    
    // Para yatırma simülasyonu
    printf("Lutfen paranizi yatirma bolmesine yerlestiriniz...\n");
    delay(2000);
    printf("Paraniz sayiliyor...\n");
    delay(2000);
    
    account->balance += amount;
    add_transaction(account, "Para Yatirma", amount, "");
    printf("\n%.2f TL yatirildi.\n", amount);
    printf("Yeni bakiye: %.2f TL\n", account->balance);
    delay(2000);
    return true;
}

// Para transferi işlemi
bool transfer(ATM *atm, BankAccount *sender, BankAccount *receiver, float amount) {
    if (sender->is_locked) {
        printf("Hesabiniz gecici olarak kilitlenmistir. Bankanizla iletisime gecin.\n");
        delay(2000);
        return false;
    }
    
    if (amount <= 0) {
        printf("Gecersiz miktar girdiniz.\n");
        delay(2000);
        return false;
    }
    
    if (amount > sender->balance) {
        printf("Yetersiz bakiye!\n");
        delay(2000);
        return false;
    }
    
    if (amount > 20000) { // Günlük transfer limiti
        printf("Gunluk transfer limitiniz 20,000 TL'dir.\n");
        delay(2000);
        return false;
    }
    
    sender->balance -= amount;
    receiver->balance += amount;
    add_transaction(sender, "Para Transferi (Gonderilen)", amount, receiver->account_no);
    add_transaction(receiver, "Para Transferi (Alinan)", amount, sender->account_no);
    
    printf("\n%.2f TL, %s %s adli kisiye transfer edildi.\n", 
           amount, receiver->name, receiver->surname);
    printf("Yeni bakiye: %.2f TL\n", sender->balance);
    delay(2000);
    return true;
}

// İşlem geçmişini göster
void view_transaction_history(BankAccount *account) {
    system("clear || cls");
    printf("\n==================================================\n");
    printf("                ISLEM GECMISI                    \n");
    printf("==================================================\n");
    
    if (account->transaction_count == 0) {
        printf("Henuz islem gecmisi bulunmamaktadir.\n");
    } else {
        int start = (account->transaction_count > MAX_TRANSACTIONS) ? 
                   account->transaction_count - MAX_TRANSACTIONS : 0;
        
        for (int i = start; i < account->transaction_count; i++) {
            printf("%d. %s - %s\n", i+1, account->transactions[i].date, 
                   account->transactions[i].type);
            printf("   Miktar: %.2f TL\n", account->transactions[i].amount);
            if (strlen(account->transactions[i].target_account) > 0) {
                printf("   Hedef Hesap: %s\n", account->transactions[i].target_account);
            }
            printf("   Bakiye: %.2f TL\n", account->transactions[i].balance_after);
            printf("--------------------------------------------------\n");
        }
    }
    
    printf("==================================================\n\n");
    delay(3000);
}

// Şifre değiştirme
bool change_password(BankAccount *account, const char *new_password) {
    if (strlen(new_password) != 5) {
        printf("Sifre 5 haneli rakamlardan olusmalidir.\n");
        delay(2000);
        return false;
    }
    
    strcpy(account->password, new_password);
    printf("Sifreniz basariyla degistirildi.\n");
    delay(1000);
    return true;
}

// Giriş işlemi
bool login(BankAccount *account, const char *entered_password) {
    if (account->is_locked) {
        printf("Bu hesap kilitlenmis. Bankanizla iletisime gecin.\n");
        delay(2000);
        return false;
    }
    
    if (strcmp(entered_password, account->password) == 0) {
        account->login_attempts = 0;
        get_current_datetime(account->last_login);
        return true;
    } else {
        account->login_attempts++;
        if (account->login_attempts >= 3) {
            account->is_locked = true;
            printf("3 basarisiz giris denemesi. Hesabiniz kilitlendi.\n");
        } else {
            printf("Yanlis sifre! Kalan deneme hakki: %d\n", 3 - account->login_attempts);
        }
        return false;
    }
}

// Hesap bulma
BankAccount* find_account(ATM *atm, const char *account_no) {
    for (int i = 0; i < atm->account_count; i++) {
        if (strcmp(atm->accounts[i].account_no, account_no) == 0) {
            return &atm->accounts[i];
        }
    }
    return NULL;
}

// İşlem ekleme
void add_transaction(BankAccount *account, const char *type, float amount, const char *target_account) {
    if (account->transaction_count >= MAX_TRANSACTIONS) {
        // Eski işlemleri kaydırmak için
        for (int i = 0; i < MAX_TRANSACTIONS - 1; i++) {
            account->transactions[i] = account->transactions[i+1];
        }
        account->transaction_count = MAX_TRANSACTIONS - 1;
    }
    
    Transaction *t = &account->transactions[account->transaction_count];
    get_current_datetime(t->date);
    strcpy(t->type, type);
    t->amount = amount;
    t->balance_after = account->balance;
    strcpy(t->target_account, target_account);
    
    account->transaction_count++;
}

// Şuanki tarih ve saati al
void get_current_datetime(char *buffer) {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// Giriş bufferını temizle
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// Gecikme fonksiyonu (basit bir implementasyon)
void delay(unsigned int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #endif
}
