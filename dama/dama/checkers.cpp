#include <iostream> 
#include <fstream> 
#include <vector>

using namespace std;

//Sıranın kimde olduğunu tuttuğum değişken
char turn;
//Eğer bir yeme işlemi gerçekleşirse true atayıp işlem yaptığım değişken
bool dead = false;

//fonksiyonların prototypelerinin tanımlandığı kısım
int columnValue(char ch);
void setTable();
vector<vector<char>> getTable();
void printTable(vector<vector<char>> vec);
void updateTable(int thisRow, int nextRow, int thisCol, int nextCol);
vector<vector<char>> move(vector<vector<char>> vec, int black, int white, string input);
void writeMoves(string move, int thisRow, int nextRow, int thisCol, int nextCol);
bool checkMove(vector<vector<char>> vec, int thisRow, int nextRow, int thisCol, int nextCol, int black, int white);
void updateBlack();
void updateWhite();
int getWhiteScores();
int getBlackScores();

//Bir adım oynandığında adım kodunu 'a3b3' gibi ve oynayan taşın renk kodunu yeni bir dosyaya yazan fonksiyon
void writeMoves(string move, int thisRow, int nextRow, int thisCol, int nextCol) {

    //currentChar hangi taşla oynandığını tutmak için kullanılan değişken w veya b yazmak için hangi taşla adım atıldığını kontrol ediyorum
    char currentChar;
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in | ios::app);
    //dosyada hareket edecek taşın olduğunu noktaya dosya pointerını hareket ettirip değeri okuyorum
    tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
    tableFile.read((char*)&currentChar, sizeof(char));
    tableFile.close();
    char doubleDot = ':';
    //her yazma işleminden sonra bir alt satıra geçmek için \n stringini de dosyaya yazdırıyorum
    string row = "\n";
    fstream movesFile("move.dat", ios::binary | ios::out | ios::in | ios::app);
    movesFile.write((char*)&currentChar, sizeof(char));
    movesFile.write((char*)&doubleDot, sizeof(char));
    
    //fonksiyona parametre olarak aldığım move yani hamle stringini dosyaya yazdırıyorum
    movesFile.write(move.c_str(), move.size());
    movesFile.write(row.c_str(), row.size());
    movesFile.close();
}

//txt dosyamdaki taşları char türünden iki boyutlu vectöre aktardığım fonksiyon
vector<vector<char>> getTable() {
   
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);

   //dosyadan okuduğum değişkenleri geçici olarak tuttuğum değişken
    char temp;

    //değişkenleri aktardığım vectör
    vector<vector<char>> vec(8);

    //txt dosyamda 9 satır olduğu için 9 kere dönen for döngüsü
    for (int i = 0; i < 9; i++) {
        //her satırda maksimum 24 karakter olduğu için 25 ve katları ile satır başlarına gidiyorum
        tableFile.seekg(i * 25, ios::beg);
      
        //txt dosyamda 24 sütun olduğu için 24 kere dönen for döngüsü
        for (int j = 0; j < 24; j++) {

            //ilk 8 satırda sadece taşlar olduğu için bu kontrolü koyup vectöre atamayı sadece bu kısımda yapıyorum
            if (i < 8) {
                
               //dosyadan değer okuduğum işlem
                tableFile.read((char*)&temp, sizeof(char));

                //bosluk değerlerini vectöre atamamak için yaptığım kontrol
                if (temp != ' ') {

                    if (temp == '*') {
                        //dama olmayan taşların ikinci karakterlerini * olarak tuttuğum için aynı aktarımı vectöre de yaptım
                        vec[i].push_back('*');                       
                    }
                    else
                        vec[i].push_back(temp);
                    
                }  
            }
            else {
                //son satırda sıra değişkenini tek karakterde tuttuğum için bu değişkeni alıp işlemi sonlandırdım
                tableFile.read((char*)&turn, sizeof(char));
                break;
            }
        }
    }
    tableFile.close();
    return vec;
}

//vektörün elemanlarını tahta düzeninde ekrana bastırdığım fonksiyon
void printTable(vector<vector<char>> vec) {
    
    cout << "........................................." << endl;
    cout << "\t\tCHECKERS\n.........................................\n" << endl;

    for (int i = 0; i < 8; i++) {
        cout << i + 1 << "    ";
        for (int j = 0; j < 16; j++) {
            cout << vec[i][j];
            if (j > 0 && j % 2 == 1)
                cout << "  ";
        }
        cout <<endl<<endl;
    }
    cout <<endl<< "     A " << "  B " << "  C " << "  D " << "  E " << "  F " << "  G " << "  H "<<endl<<endl;
    //Oynama sırasının kimde olduğunu ekrana bastırdığım işlem
   
}

//hareket işlemi mümkünse gerekli işlemleri yapan fonksiyon
vector<vector<char>> move(vector<vector<char>> vec,int black,int white, string input) {

    //tüm koordinatı string olarak okuduğum için rakamları integer değere dönüştürdüm 
    //şuanki konumu ve gideceği konumu satır ve sütun olarak ayırdım
    int thisRow = int(input[1] - '0');
    int nextRow = int(input[3] - '0');
    int thisCol = columnValue(input[0]);
    int nextCol = columnValue(input[2]);

    //checkMove fonksiyonu ile hareketin mümkün olup olmadığını kontrol ediyorum eğer mümkünse gerekli işlemler ifin içinde gerçekleşiyor
    if (checkMove(vec,thisRow, nextRow, thisCol, nextCol, black , white)) {

        //hareket edilebiliyorsa hareket koordinatları moves.txt dosyasına yazılıyor
        writeMoves(input, thisRow, nextRow, thisCol, nextCol);
        //bu fonksiyon ile birlikte eğer hareket mümkünse dosyada gerekli konumlar güncelleniyor
        updateTable(thisRow, nextRow, thisCol, nextCol);
        //güncellenen dosyadan veri çekilerek vectör de güncelleniyor
        vec = getTable();
        //vectör tahta düzeni ile ekrana basılıyor 
        printTable(vec);
        //siyah taş sayısını dosyadan alan fonksiyon
        black = getBlackScores();
        //beyaz taş sayısını dosyadan alan fonksiyon
        white = getWhiteScores();
        //ve taş sayılarını her hareketten sonra ekrana basan kod 
    
    }else{
        //eğer girilen hareket mümkün değilse de ekrana yanlış hamle uyarısı basılıp tekrar işlem yapılması isteniyor
        if (input != "MENU") {
            cout << "Wrong command ! Please enter again." << endl<<endl;
        }
    }

   return vec;
}

//hareket işlemleri mümkünse true değilse false işlemi yapan fonksiyon
bool checkMove(vector<vector<char>> vec, int thisRow, int nextRow, int thisCol, int nextCol, int black, int white) {

    fstream myFile("game.dat", ios::binary | ios::out | ios::in);
    //şuanki karakteri tutan değişken
    char currentChar;
    //gidilecek karelerin taşlarını atadığım değişkenler
    char enemy1;
    char enemy2;

    //hamle yapacak değişkeni okuma
    myFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
    myFile.read((char*)&currentChar, sizeof(char));

    //sıra beyazdaysa
    if (turn == 'W') {

        //eğer normal taş oynuyorsa
        if (currentChar == 'w') {

            //satırlar eşitse sadece sütun hareketi yapmasını sağlayan kontrol
            if (thisRow == nextRow) {

                //2 sütun farkı eğer taş yediyse olabilir bu durumu kontrol ediyor
                if (abs(thisCol - nextCol) == 2) {

                    //eğer son sütundaysa sadece sola hareket etmesini kontrol ediyor
                    if (thisCol == 7) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'b' || enemy1 == 'B') {
                            
                            updateBlack();
                            dead = true;                       
                            return true;
                        }
                        else {
                            return false;
                        }

                    }//eğer ilk sütundaysa sadece sağa hareket etmesini kontrol ediyor
                    else if (thisCol == 0) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'b' || enemy1 == 'B') {

                            updateBlack();
                            dead = true;
                            return true;
                        }
                        else {

                            return false;
                        }
                    }//diğer tüm sütunlar için normal hareketleri kontrol ediyor
                    else {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];
                        enemy2 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'b' || enemy1 == 'B' || enemy2 == 'b' || enemy2 == 'B') {
                                             
                            updateBlack();
                            dead = true;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }//taş yemeden sadece 1 sütun hareket edebilir bu işlemi kontrol ediyor
                else if (abs(thisCol - nextCol) == 1) {

                    //eğer son sütundaysa sadece sola hareket etmesini kontrol ediyor
                    if (thisCol == 7) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer gideceği kare boşsa
                        if (enemy1 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else {
                            return false;
                        }

                    }//eğer ilk sütundaysa sadece sağa hareket etmesini kontrol ediyor
                    else if (thisCol == 0) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];

                        //eğer gideceği kare boşsa
                        if (enemy1 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else {
                            return false;
                        }
                    }//diğer tüm sütunlar için normal hareketleri kontrol ediyor
                    else {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];
                        enemy2 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer gideceği kare boşsa
                        if (enemy1 == '-' || enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }
                else
                    return false;

            }//eğer sütunlar aynıysa sadece satır hareketini kontrol ediyor
            else if (thisCol == nextCol) {

                //2 satır farkı eğer taş yediyse olabilir bu durumu kontrol ediyor
                if (nextRow - thisRow == 2) {

                    //eğer son satıra giderken yeme işlemi yapıldıysa kontrol ediyor
                    if (thisRow == 6) {

                        //eğer rakip taş üstünden atlanılan karedeyse kontrol ediyor
                        if (vec[thisRow - 1 + 1][thisCol * 2] == 'b' || vec[thisRow - 1 + 1][thisCol * 2] == 'B') {
                            
                            updateBlack();
                            dead = true;                        
                            return true;
                        }
                        else
                            return false;
                    } // diğer yeme işlemleri kontrol ediyor
                    else {

                        enemy1 = vec[thisRow - 1 + 1][thisCol * 2];

                        //eğer rakip taş üstünden atlanılan karedeyse kontrol ediyor
                        if (enemy1 == 'b' || enemy1 == 'B') {

                            updateBlack();
                            dead = true;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }//yeme işlemi yoksa tek satır ilerlemeyi kontrol ediyor
                else if (nextRow - thisRow == 1) {

                    enemy1 = vec[thisRow - 1 + 1][thisCol * 2];

                    //eğer son satırdan bir önceki satırdaysa 
                    if (thisRow == 7) {

                        return true;
                    }

                    //gideceği yer boşsa true döndürüyor
                    if (enemy1 == '-') {

                        myFile.seekg(8 * 25, ios::beg);
                        turn = 'B';
                        myFile.write((char*)&turn, sizeof(char));
                        return true;
                    }
                    else {
                        return false;
                    }

                }
                else {

                    return false;
                }
            }
            else {
                return false;
            }
        }//eğer dama olan beyaz taş ise
        else if (currentChar == 'W') {

            //satırlar eşitse sadece sütun hareketi yapmasını sağlayan kontrol
            if (thisRow == nextRow) {

                //eğer sola doğru bir hareket varsa
                if (thisCol > nextCol) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = ((thisCol -1) * 2) ; i > nextCol * 2; i--) {
                      
                        enemy1 = vec[thisRow - 1][i];
                      
                        if (enemy1 == 'b' || enemy1 == 'B' || enemy1 == 'w') {

                            flag++;                          
                        }
                    }
                    
                    //eğer flag 0 ise yeme işlemi gerçekleşmeden ilerleme hareketi 
                    if (flag == 0) {

                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[thisRow - 1][nextCol * 2 + 2];
                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'B' || enemy1 == 'b')) {
                            
                            updateBlack();
                            dead = true;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }//eğer sağa doğru bir hareket varsa
                else if (nextCol > thisCol) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = (thisCol + 1)* 2; i < ((nextCol) * 2); i++) {

                        enemy1 = vec[thisRow - 1][i];
                
                        if (enemy1 == 'b' || enemy1 == 'B' || enemy1 == 'w') {

                            flag++;
                        }

                    }
                  
                    //eğer flag 0 ise yeme işlemi gerçekleşmeden ilerleme hareketi 
                    if(flag == 0) {

                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[thisRow - 1][nextCol * 2 - 2];
                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'B' || enemy1 == 'b')) {

                            updateBlack();
                            dead = true;
                            return true;
                        }                  
                        else
                            return false;
                    }
                    else
                        return false;

                }
            }//eğer sütunlar eşit ise satır işlemleri kontrol ediliyor
            else if (thisCol == nextCol) {

                //eğer yukarı doğru hareket ediliyorsa
                if (thisRow > nextRow) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = thisRow - 2 ; i >= nextRow; i--) {

                        enemy1 = vec[i][thisCol * 2];
                
                        if (enemy1 == 'b' || enemy1 == 'B' || enemy1 == 'w') {

                            flag++;
                        }
                    }

                    //eğer flag 0 ise yeme işlemi gerçekleşmeden ilerleme hareketi 
                    if (flag == 0) {

                        enemy2 = vec[nextRow - 1][thisCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[nextRow - 1+1][thisCol * 2];
                        enemy2 = vec[nextRow - 1][thisCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'B' || enemy1 == 'b')){
                      
                            updateBlack();
                            dead = true;
                            return true;
                        }    
                        else
                            return false;
                    }
                    else
                        return false;
                }//eğer aşşağı doğru bir hareket varsa
                else if (nextRow > thisRow) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = thisRow ; i < nextRow; i++) {

                        enemy1 = vec[i][thisCol * 2];

                        if (enemy1 == 'b' || enemy1 == 'B' || enemy1 == 'w') {

                            flag++;
                        }

                    }
               
                    //eğer flag 0 ise yeme işlemi gerçekleşmeden ilerleme hareketi 
                    if (flag == 0) {

                        enemy2 = vec[nextRow - 1][nextCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'B';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[nextRow - 2][nextCol * 2];
                        enemy2 = vec[nextRow - 1][nextCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'B' || enemy1 == 'b')) {
                            
                            updateBlack();
                            dead = true;                       
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }
            }
            else {
                return false;
            }
        }
    }//sıra siyahdaysa
    else if (turn == 'B') {
        
        //eğer normal taş oynuyorsa
        if (currentChar == 'b') {

            //satırlar eşitse sadece sütun hareketi yapmasını sağlayan kontrol
            if (thisRow == nextRow) {

                //2 sütun farkı eğer taş yediyse olabilir bu durumu kontrol ediyor
                if (abs(thisCol - nextCol) == 2) {

                    //eğer son sütundaysa sadece sola hareket etmesini kontrol ediyor
                    if (thisCol == 7) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'w' || enemy1 == 'W') {
                                                   
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else {
                            return false;
                        }

                    }//eğer ilk sütundaysa sadece sağa hareket etmesini kontrol ediyor
                    else if (thisCol == 0) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'w' || enemy1 == 'W') {
                                           
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else {

                            return false;
                        }
                    }//diğer tüm sütunlar için normal hareketleri kontrol ediyor
                    else {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];
                        enemy2 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'w' || enemy1 == 'W' || enemy2 == 'w' || enemy2 == 'W') {
                                             
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }//taş yemeden sadece 1 sütun hareket edebilir bu işlemi kontrol ediyor
                else if (abs(thisCol - nextCol) == 1) {

                    //eğer son sütundaysa sadece sola hareket etmesini kontrol ediyor
                    if (thisCol == 7) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer gideceği kare boşsa
                        if (enemy1 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else {
                            return false;
                        }

                    }//eğer ilk sütundaysa sadece sağa hareket etmesini kontrol ediyor
                    else if (thisCol == 0) {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];

                        //eğer gideceği kare boşsa
                        if (enemy1 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            cout << "burdaa" << endl;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }//diğer tüm sütunlar için normal hareketleri kontrol ediyor
                    else {

                        enemy1 = vec[thisRow - 1][thisCol * 2 + 2];
                        enemy2 = vec[thisRow - 1][thisCol * 2 - 2];

                        //eğer gideceği kare boşsa
                        if (enemy1 == '-' || enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }
                else
                    return false;

            }//eğer sütunlar aynıysa sadece satır hareketini kontrol ediyor
            else if (thisCol == nextCol) {

                //2 satır farkı eğer taş yediyse olabilir bu durumu kontrol ediyor
                if (thisRow - nextRow == 2) {

                    //eğer ilk satıra giderken yeme işlemi yapıldıysa kontrol ediyor
                    if (thisRow == 3) {

                        if (vec[thisRow - 1 - 1][thisCol * 2] == 'w' || vec[thisRow - 1 - 1][thisCol * 2] == 'W') {

                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else
                            return false;
                    }//diğer satır hareketleri için kontrol
                    else {

                        enemy1 = vec[thisRow - 1 - 1][thisCol * 2];

                        //eğer rakip taş varsa true yoksa false döndürüyor ve taş sayısını güncelliyor
                        if (enemy1 == 'w' || enemy1 == 'W') {
                                                   
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }//eğer yeme işlemi yapmadan hareket varsa
                else if (thisRow - nextRow == 1) {

                    enemy1 = vec[thisRow - 1 - 1][thisCol * 2];

                    if (thisRow == 1) {

                        return true;
                    }

                    //eğer gideceği kare boşsa
                    if (enemy1 == '-') {

                        myFile.seekg(8 * 25, ios::beg);
                        turn = 'W';
                        myFile.write((char*)&turn, sizeof(char));
                        return true;
                    }
                    else {
                        return false;
                    }
                }
                else {

                    return false;
                }
            }
            else {
                return false;
            }
        }//eğer sıra siyah dama taştaysa
        else if (currentChar == 'B') {

            //satırlar eşitse sadece sütun hareketi yapmasını sağlayan kontrol
            if (thisRow == nextRow) {

                //eğer sola doğru bir hareket varsa
                if (thisCol > nextCol) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = ((thisCol - 1) * 2); i > nextCol * 2; i--) {

                        enemy1 = vec[thisRow - 1][i];                 

                        if (enemy1 == 'w' || enemy1 == 'W' || enemy1 == 'b') {

                            flag++;
                        }

                    }

                    //eğer flag 0 ise yeme işlemi gerçekleşmeden ilerleme hareketi 
                    if (flag == 0) {

                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[thisRow - 1][nextCol * 2 + 2];
                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'W' || enemy1 == 'w')) {
                            
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }//eğer sağa doğru bir hareket varsa
                else if (nextCol > thisCol) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = (thisCol + 1) * 2; i < ((nextCol) * 2); i++) {
                  
                        enemy1 = vec[thisRow - 1][i];

                        if (enemy1 == 'w' || enemy1 == 'W' || enemy1 == 'b') {

                            flag++;
                        }
                    }
      
                    //eğer flag 0 ise yeme işlemi gerçekleşmeden ilerleme hareketi 
                    if (flag == 0) {

                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[thisRow - 1][nextCol * 2 - 2];
                        enemy2 = vec[thisRow - 1][nextCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'W' || enemy1 == 'w')) {
                           
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }
            }//eğer sütunlar eşit ise satır işlemleri kontrol ediliyor
            else if (thisCol == nextCol) {

                //eğer yukarı doğru hareket ediliyorsa
                if (thisRow > nextRow) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = thisRow - 2; i >= nextRow; i--) {

                        enemy1 = vec[i][thisCol * 2];

                        if (enemy1 == 'w' || enemy1 == 'W' || enemy1 == 'b') {

                            flag++;
                        }
                    }
    
                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    if (flag == 0) {

                        enemy2 = vec[nextRow - 1][thisCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[nextRow - 1 + 1][thisCol * 2];
                        enemy2 = vec[nextRow - 1][thisCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'W' || enemy1 == 'w')) {
                            
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }//eğer aşşağı doğru hareket ediliyorsa
                else if (nextRow > thisRow) {

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    int flag = 0;
                    for (int i = thisRow; i < nextRow; i++) {

                        enemy1 = vec[i][thisCol * 2];

                        if (enemy1 == 'w' || enemy1 == 'W' || enemy1 == 'b') {

                            flag++;
                        }
                    }

                    //üzerinden geçilen elemanları tarayan for döngüsü ve eğer boş olmayan kareler varsa flag artıyor
                    if (flag == 0) {

                        enemy2 = vec[nextRow - 1][nextCol * 2];

                        if (enemy2 == '-') {

                            myFile.seekg(8 * 25, ios::beg);
                            turn = 'W';
                            myFile.write((char*)&turn, sizeof(char));
                            return true;
                        }
                        else
                            return false;
                    }//eğer flag 1 ise yeme işlemi olmuştur ve ilerleme yapılır
                    else if (flag == 1) {

                        enemy1 = vec[nextRow - 2][nextCol * 2];
                        enemy2 = vec[nextRow - 1][nextCol * 2];

                        if (enemy2 == '-' && (enemy1 == 'W' || enemy1 == 'w')) {
                           
                            updateWhite();
                            dead = true;
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
        myFile.close();
}

//New game yapılacağı zaman dosyaya tekrardan aynı tablo düzenini bir vektör vasıtasıyla yazan fonksiyon
void setTable() {

    vector<vector<char>> table(9);

    //tablo düzenine uygun bir şekilde vektör oluşturulup ataması yapılıyor
    for(int i = 0 ; i<9 ;i++){
        for (int j = 0; j < 24; j++) {

            if (i == 8) {
                table[i].push_back('W');
                break;
            }
            if (i == 1 || i == 2) {
                if (j % 3 == 0) {
                    table[i].push_back('w');
                }
                else if(j%3 == 1) {
                    table[i].push_back('*');
                }else
                    table[i].push_back(' ');
            }
            else if (i == 5 || i == 6) {
                if (j % 3 == 0) {
                    table[i].push_back('b');
                }
                else if (j % 3 == 1) {
                    table[i].push_back('*');
                }
                else
                    table[i].push_back(' ');
            }
            else {
                if (j % 3 == 2) {
                    table[i].push_back(' ');
                }
                else {
                    table[i].push_back('-');
                }
            }            
        }
        table[i].push_back('\n');
    }

    //vektör uygun bir şekilde txt dosyamıza yazılıyor
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);
    
     char temp;
        vector<vector<char>> vec(8);
        for (int i = 0; i < 9; i++) {
            tableFile.seekg(i * 25, ios::beg);
        
            for (int j = 0; j < 25; j++) {

                if (i < 8) {
                    temp = table[i][j];
                    tableFile.write((char*)&temp, sizeof(char));
                }
                else {
                    temp = table[i][j];
                    tableFile.write((char*)&temp, sizeof(char));
                    break;
                }
            }
        }
        //taş sayılarını txt dosyasına yazdığım kısım
        tableFile.seekg(8 * 25 + 2);
        char onlarBas = '1';
        char birlerBas = '6';
        tableFile.write((char*)&onlarBas, sizeof(char));
        tableFile.write((char*)&birlerBas, sizeof(char));
        tableFile.seekg(8 * 25 + 5);
        tableFile.write((char*)&onlarBas, sizeof(char));
        tableFile.write((char*)&birlerBas, sizeof(char));
        tableFile.close();
}

//dosya üzerinde herhangi bir işlem ve hamle olduğunda txt dosyasını güncelleyen fonksiyon
void updateTable(int thisRow, int nextRow, int thisCol, int nextCol) {
    
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);
    char currentChar1;
    char currentChar2;
    //hareket edilen boş noktaya atama yapan kodlar '-' olmasının sebebi boş taşları txt dosyamda '-' ile tutuyorum
    char nextChar1 ='-';
    char nextChar2 ='-';

    //iki karakter şeklinde hareket edecek olan taşı okuyan işlemler
    tableFile.seekg((((thisRow-1) * 25) + (thisCol*3)), ios::beg);
    tableFile.read((char*)&currentChar1, sizeof(char));
    tableFile.read((char*)&currentChar2, sizeof(char));

    //eğer hareket eden karakter b ise ona özel kontrol yaptığım kısım
    if (currentChar1 == 'b') {

        //eğer yeme işlemi gerçekleştiyse
        if (dead) {

            //eğer yukarı doğru bir hareket varsa
            if (nextRow < thisRow) {
                //gidilen karenin bir önündeki eleman yenmiş olacağı için '-' atıyorum yani o karelerin artık eleman içermediğini işaretliyorum
                tableFile.seekg((((thisRow - 1 - 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
               
            }//eğer sola doğru bir hareket varsa
            else if (thisCol > nextCol) {
                //gidilen karenin bir önündeki eleman yenmiş olacağı için '-' atıyorum yani o karelerin artık eleman içermediğini işaretliyorum
                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)-3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
            
            }//normal taşlar geri adam atamayacağı için eğer sağa doğru bir hareket varsa
            else {
                //gidilen karenin bir önündeki eleman yenmiş olacağı için '-' atıyorum yani o karelerin artık eleman içermediğini işaretliyorum
                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)+3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
               
            }
        }
        //yeme işlemi olmayan kısımlar 
        //eğer en üst satıra çıkarsa taşı damaya dönüştürüyorum
        if (nextRow == 1) {

            char currentChar1 = 'B';
            char currentChar2 = 'B';
            //gidilen yere taş ataması yapıyorum geldiği karelere de artık eleman içermediğini işaretliyorum
            tableFile.seekp((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
            tableFile.write((char*)&currentChar1, sizeof(char));
            tableFile.write((char*)&currentChar2, sizeof(char));

            tableFile.seekp((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
            tableFile.write((char*)&nextChar1, sizeof(char));
            tableFile.write((char*)&nextChar2, sizeof(char));
        }
        else {
            //dama olmadığı ve yemediği diğer tüm hareketler için gidilen yere taş ataması yapıyorum geldiği karelere de artık eleman içermediğini işaretliyorum
            tableFile.seekp((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
            tableFile.write((char*)&currentChar1, sizeof(char));
            tableFile.write((char*)&currentChar2, sizeof(char));

            tableFile.seekp((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
            tableFile.write((char*)&nextChar1, sizeof(char));
            tableFile.write((char*)&nextChar2, sizeof(char));
        }
     
    }
    else if (currentChar1 == 'w') {

        //eğer yeme işlemi gerçekleştiyse
        if (dead) {

            //eğer aşşağı doğru bir hareket varsa
            if (nextRow > thisRow) {
                //gidilen karenin bir önündeki eleman yenmiş olacağı için '-' atıyorum yani o karelerin artık eleman içermediğini işaretliyorum
                tableFile.seekg((((thisRow - 1 + 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
              
            }//eğer sola doğru bir hareket varsa
            else if (thisCol > nextCol) {
                //gidilen karenin bir önündeki eleman yenmiş olacağı için '-' atıyorum yani o karelerin artık eleman içermediğini işaretliyorum
                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3) - 3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
                
            }//normal taşlar geri adam atamayacağı için eğer sağa doğru bir hareket varsa
            else {
                //gidilen karenin bir önündeki eleman yenmiş olacağı için '-' atıyorum yani o karelerin artık eleman içermediğini işaretliyorum
                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3) + 3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
              
            }
        }
        //yeme işlemi olmayan kısımlar 
        //eğer en alt satıra giderse taşı damaya dönüştürüyorum
        if (nextRow == 8) {

            char currentChar1 = 'W';
            char currentChar2 = 'W';
            //gidilen yere taş ataması yapıyorum geldiği karelere de artık eleman içermediğini işaretliyorum
            tableFile.seekp((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
            tableFile.write((char*)&currentChar1, sizeof(char));
            tableFile.write((char*)&currentChar2, sizeof(char));

            tableFile.seekp((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
            tableFile.write((char*)&nextChar1, sizeof(char));
            tableFile.write((char*)&nextChar2, sizeof(char));
        }
        else {
            //dama olmadığı ve yemediği diğer tüm hareketler için gidilen yere taş ataması yapıyorum geldiği karelere de artık eleman içermediğini işaretliyorum
            tableFile.seekp((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
            tableFile.write((char*)&currentChar1, sizeof(char));
            tableFile.write((char*)&currentChar2, sizeof(char));

            tableFile.seekp((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
            tableFile.write((char*)&nextChar1, sizeof(char));
            tableFile.write((char*)&nextChar2, sizeof(char));
        }
    }
    //Eğer oynayan taşlar dama ise hareket yönleri aynı olabileceği için tek if içinde değerlendirdim
    else if (currentChar1 == 'W' || currentChar1 == 'B') {
    
        char nextChar1 = '-';
        char nextChar2 = '-';
        //eğer yeme işlemi gerçekleştiyse
        if (dead) {

            //eğer aşşağı doğru bir hareket varsa
            if (nextRow > thisRow) {

                //gidilen karelere dama taşlarını atıyorum , yediği ve geldiği konumlar boş kaldığı için o karelere de '-' atayak boş olduğunu işaretliyorum
                tableFile.seekg((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
                tableFile.write((char*)&currentChar1, sizeof(char));
                tableFile.write((char*)&currentChar2, sizeof(char));

                tableFile.seekg((((nextRow - 1 - 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

            }//eğer yukarı doğru bir hareket varsa
            else if(nextRow < thisRow){

                //gidilen karelere dama taşlarını atıyorum , yediği ve geldiği konumlar boş kaldığı için o karelere de '-' atayak boş olduğunu işaretliyorum
                tableFile.seekg((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
                tableFile.write((char*)&currentChar1, sizeof(char));
                tableFile.write((char*)&currentChar2, sizeof(char));

                tableFile.seekg((((nextRow - 1 + 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

            }//eğer sola doğru bir hareket varsa
            else if (thisCol > nextCol) {

                //gidilen karelere dama taşlarını atıyorum , yediği ve geldiği konumlar boş kaldığı için o karelere de '-' atayak boş olduğunu işaretliyorum
                tableFile.seekg((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
                tableFile.write((char*)&currentChar1, sizeof(char));
                tableFile.write((char*)&currentChar2, sizeof(char));

                tableFile.seekg((((thisRow - 1) * 25) + (nextCol * 3) + 3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

            }//eğer sağa doğru bir hareket varsa
            else if(nextCol > thisCol) {

                //gidilen karelere dama taşlarını atıyorum , yediği ve geldiği konumlar boş kaldığı için o karelere de '-' atayak boş olduğunu işaretliyorum
                tableFile.seekg((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
                tableFile.write((char*)&currentChar1, sizeof(char));
                tableFile.write((char*)&currentChar2, sizeof(char));

                tableFile.seekg((((thisRow - 1) * 25) + (nextCol * 3) - 3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));
            }
            else {

                tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3) + 3), ios::beg);
                tableFile.write((char*)&nextChar1, sizeof(char));
                tableFile.write((char*)&nextChar2, sizeof(char));

            }
        }//yeme işlemi yoksa gidilen noktaya dama taşlarını önceki konuma da '-' atayak boş olduğunu işaretliyorum
        else {
            tableFile.seekg((((nextRow - 1) * 25) + (nextCol * 3)), ios::beg);
            tableFile.write((char*)&currentChar1, sizeof(char));
            tableFile.write((char*)&currentChar2, sizeof(char));

            tableFile.seekg((((thisRow - 1) * 25) + (thisCol * 3)), ios::beg);
            tableFile.write((char*)&nextChar1, sizeof(char));
            tableFile.write((char*)&nextChar2, sizeof(char));
        }
      
    }

    tableFile.close();
}

//Beyaz taş sayısını döndüren fonksiyon
int getWhiteScores() {

    char onlarBasWhite , birlerBasWhite;
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);
    tableFile.seekg(8 * 25 + 5);//beyaz taş sayısının olduğu konuma gidip okuyan fonksiyon
    tableFile.read((char*)&onlarBasWhite, sizeof(char));
    tableFile.read((char*)&birlerBasWhite, sizeof(char));
    int white = int(onlarBasWhite - '0') * 10 + int(birlerBasWhite - '0');//string değeri integer değere dönüştürüyorum
    tableFile.close();
    return white;
}

//Siyah taş sayısını döndüren fonksiyon
int getBlackScores() {
    char onlarBasBlack, birlerBasBlack;
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);
    tableFile.seekg(8 * 25 + 2);//siyah taş sayısının olduğu konuma gidip okuyan fonksiyon
    tableFile.read((char*)&onlarBasBlack, sizeof(char));
    tableFile.read((char*)&birlerBasBlack, sizeof(char));
    int black = int(onlarBasBlack - '0') * 10 + int(birlerBasBlack - '0');//string değeri integer değere dönüştürüyorum
    tableFile.close();
    return black;
}

//beyaz bir taş siyah taşı yediği zaman siyah taş sayısını dosyada azaltan fonksiyon
void updateBlack() {

    char onlarBasBlack, birlerBasBlack;
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);
    tableFile.seekg(8 * 25 + 2);//siyah taş sayısının olduğu konuma gidip okuyan fonksiyon
    tableFile.read((char*)&onlarBasBlack, sizeof(char));
    tableFile.read((char*)&birlerBasBlack, sizeof(char));
    int black = int(onlarBasBlack - '0') * 10 + int(birlerBasBlack - '0');
    black--;
    //eğer tek basamaksa başına 0 atayıp string şeklinde dosyada güncelliyorum
    if (black < 10) {
        tableFile.seekg(8 * 25 + 2);
        char temp = '0';
        char num = char(black+ '0');
    
        tableFile.write((char*)&temp, sizeof(char));
        tableFile.write((char*)&num, sizeof(char));
        tableFile.close();

    }
    else {
        tableFile.seekg(8 * 25 + 2);
        int temp = int(birlerBasBlack - '0');
        temp--;
        char birler = char(temp + '0');
        tableFile.write((char*)&onlarBasBlack, sizeof(char));
        tableFile.write((char*)&birler, sizeof(char));
        tableFile.close();
    }
}

//siyah bir taş beyaz taşı yediği zaman siyah taş sayısını dosyada azaltan fonksiyon
void updateWhite() {

    char onlarBasWhite, birlerBasWhite;
    fstream tableFile("game.dat", ios::binary | ios::out | ios::in);
    tableFile.seekg(8 * 25 + 5);//beyaz taş sayısının olduğu konuma gidip okuyan fonksiyon
    tableFile.read((char*)&onlarBasWhite, sizeof(char));
    tableFile.read((char*)&birlerBasWhite, sizeof(char));
    int white = int(onlarBasWhite - '0') * 10 + int(birlerBasWhite - '0');
    white--;
    //eğer tek basamaksa başına 0 atayıp string şeklinde dosyada güncelliyorum
    if (white < 10) {
        tableFile.seekp(8 * 25 + 5);
        char temp = '0';
        char num = char(white + '0');
        tableFile.write((char*)&temp, sizeof(char));
        tableFile.write((char*)&num, sizeof(char));
        tableFile.close();
    }
    else {
        tableFile.seekp(8 * 25 + 5);
        int temp = int(birlerBasWhite - '0');
        temp--;
        char birler = char(temp + '0');
        tableFile.write((char*)&onlarBasWhite, sizeof(char));
        tableFile.write((char*)&birler, sizeof(char));
        tableFile.close();
    }
}

//Kullanıcıdan alınan konum koordinatlarındaki harf değerlerini rakam olarak döndüren bir fonksiyon
int columnValue(char ch) {

    if (ch == 'a') {
        return 0;
    }
    else if (ch == 'b') {
        return 1;
    }
    else if (ch == 'c') {
        return 2;
    }
    else if (ch == 'd') {
        return 3;
    }
    else if (ch == 'e') {
        return 4;
    }
    else if (ch == 'f') {
        return 5;
    }
    else if (ch == 'g') {
        return 6;
    }
    else if (ch == 'h') {
        return 7;
    }
}

int main()
{
    //taş sayılarının tutulduğu değişkenler
    int black;
    int white;

    //while kontrolünü çalıştırmak ve bölmek için kullandığım değişken
    bool run = true;

    while (run) {
        //Switc Case kontrolleri
        cout << "\t\tCHECKERS\n.........................................\n" << endl;
        cout << "1.New game" << endl;
        cout << "2.Continue game" << endl;
        cout << "3.Exit Game" << endl;
        int choice;
        cin >> choice;
        cout << endl;
        switch (choice) {
         
        //new game seçeneği
        case 1: {
            cout << ".........................................";
            cout << "\n\t\tNEW GAME" << endl;
            //yeni oyun için txt dosyasına taşları yazdırma fonksiyonu
            setTable();
            //taş sayılarını atama
            black = getBlackScores();
            white = getWhiteScores();
            //yeni oyun için dosya içeriğini temizledim
            fstream movesFile("move.dat", ios::binary | ios::out | ios::in | ios::trunc);
            movesFile.close();
            //vectöre tabloyu aktarma
            vector<vector<char>> newGameTable(8);
            newGameTable = getTable();
            printTable(newGameTable);
      
            //beraberlik ve kazanma durumlarına kadar sürekli hamle yapılmasını sağlayan while döngüsü
            while (true) {

                cout << "Black Team : " << black << " | " << "White Team : " << white << endl << endl;
                cout << "Type the 'MENU' to return to the main menu" << endl << endl;
                if (turn == 'W') {
                    cout << "Team White please enter your command." << endl;
                }
                else {
                    cout << "Team Black please enter your command." << endl;
                }
                cout << endl;

                string input;
                cin >> input;
                cout << endl;

                newGameTable = move(newGameTable, black, white, input);
                black = getBlackScores();
                white = getWhiteScores();

                if ((black == 1 && white == 1) || (black == 0) || (white == 0))
                    break;
                
                if (input == "MENU")
                    break;
            }

            if (black == 0) {
                cout << "White Team Won !" << endl;
            }
            else if (white == 0) {
                cout << "Black Team Won !" << endl;

            }
            else if (white == 1 && black == 1) {
                cout << "Draw !" << endl;
            }
            else {
                cout << "Returning to the main menu..." << endl;
            }
            break;
        }
              //continue game seçeneği
        case 2: {

            cout << ".........................................";
            cout << "\n\t\tCONTINUE GAME" << endl;
            //taş sayılarını atama
            black = getBlackScores();
            white = getWhiteScores();
            //vectöre tabloyu aktarma
            vector<vector<char>> continueGameTable(8);
            continueGameTable = getTable();
            printTable(continueGameTable);
      
            //beraberlik ve kazanma durumlarına kadar sürekli hamle yapılmasını sağlayan while döngüsü
            while (true) {

                cout << "Black Team : " << black << " | " << "White Team : " << white << endl << endl;
                cout << "Type the 'MENU' to return to the main menu" << endl << endl;
                if (turn == 'W') {
                    cout << "Team White please enter your command." << endl;
                }
                else {
                    cout << "Team Black please enter your command." << endl;
                }
                cout << endl;

                string input;
                cin >> input;
                cout << endl;

                continueGameTable = move(continueGameTable, black, white, input);
                black = getBlackScores();
                white = getWhiteScores();

                if ((black == 1 && white == 1) || (black == 0) || (white == 0))
                    break;

                if (input == "MENU")
                    break;
            }

            if (black == 0) {
                cout << "White Team Won !" << endl;
            }
            else if (white == 0) {
                cout << "Black Team Won !" << endl;

            }
            else if (white == 1 && black == 1) {
                cout << "Draw !" << endl;
            }
            else {
                cout << "Returning to the main menu..." << endl;
            }
            break;
        }

        case 3 :
            cout << "\n\t\tEXIT GAME\n.........................................\n" << endl;
            run = false;
            break;
        }
    }
  
    return 0;
}