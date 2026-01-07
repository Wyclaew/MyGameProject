========================================================================
PROJE: CIRCLE SURVIVOR - README
========================================================================


1. ÖĞRENCİ BİLGİLERİ
------------------------------------------------------------------------
Adı Soyadı      : Eren Semih YILDIZ
Öğrenci No      : 25253013
Bölüm           : Bilgisayar Mühendisliği


------------------------------------------------------------------------
2. OYUN FİKRİ VE AÇIKLAMASI
------------------------------------------------------------------------
Oyunumun adı "Circle Survivor". Bu proje, "Horde Survival" 
türünde, yukarıdan kuş bakışı oynanan bir aksiyon oyunudur.

Oyunun Temel Mekanikleri:
- Oyuncu, bir karakteri kontrol ederek üzerine gelen düşman dalgalarından kaçmaya ve
  otomatik ateş eden silah sistemiyle onları yok etmeye çalışır.
- Düşmanlar (Basic, Rusher, Tank) farklı hız ve can değerlerine sahiptir.
- Öldürülen düşmanlardan "XP Gem"leri düşer. Bunlar toplandığında oyuncu seviye atlar.
- Seviye atlandığında rastgele 3 adet güçlendirme kartı (saldırı hızı, hareket hızı, hasar vb.)
  sunulur ve oyuncu karakterini geliştirir.
- Oyun zorluğu zamanla artar (düşman sayısı ve spawn sıklığı değişir).
- Amaç, ölmeden mümkün olan en yüksek skoru elde etmektir.


------------------------------------------------------------------------
3. DERLEME VE ÇALIŞTIRMA
------------------------------------------------------------------------
Bu proje C dili ile yazılmış ve grafik kütüphanesi olarak "Raylib" kullanılmıştır.
Derleme işlemi için bilgisayarınızda GCC derleyicisi ve Raylib kütüphanesinin kurulu/tanımlı olması gerekmektedir.

Gerekli Kaynak Dosyalar:
main.c, game.c, player.c, enemy.c, projectile.c, collision.c, ui.c, 
gem.c, upgrade_pool.c, spatial_grid.c

Derleme Komutu (Terminal/CMD üzerinden proje klasöründe):
gcc main.c game.c player.c enemy.c projectile.c collision.c ui.c gem.c upgrade_pool.c spatial_grid.c -o main.exe -O2 -Wall -Wno-missing-braces -I include -L lib -lraylib -lopengl32 -lgdi32 -lwinmm

Çalıştırma:
Oluşan "main.exe" dosyasına çift tıklayarak veya terminalden ./main.exe
yazarak oyunu başlatabilirsiniz.

Not: .png formatındaki varlıkların (player_cat_walk.png vb.) exe ile aynı klasörde olduğundan emin olun.

------------------------------------------------------------------------
4. ZORUNLU İŞARETÇİ (POINTER) KULLANIMI
------------------------------------------------------------------------
Projemde işaretçiler (pointers), özellikle bellek yönetimi ve performans optimizasyonu
amacıyla kritik noktalarda kullanılmıştır:

A) Oyun Durumu (Game State) Yönetimi:
   `GameData` yapısı oldukça büyük bir veri bloğudur (tüm düşmanları, mermileri, ayarları vb. içinde tutar).
   Bu yapıyı fonksiyonlara değeriyle (by value) göndermek yerine, referansıyla (by reference) gönderdik.
   
   Örnek (main.c -> Game_Update):
   `void Game_Update(GameData *game, float dt)`
   Burada `*game` işaretçisi kullanılarak, fonksiyonun doğrudan orijinal veriyi değiştirmesi sağlanmış
   ve her karede (frame) tüm oyun verisinin kopyalanmasının önüne geçilerek performans artırılmıştır.

B) Düşman ve Obje Döngüleri:
   `enemy.c` veya `gem.c` içerisindeki güncellemelerde, dizi elemanlarına erişirken
   `Enemy *e = &game->enemies[i];` şeklinde işaretçiler kullanılmıştır. Bu sayede kod okunabilirliği
   artmış ve veriye doğrudan bellek adresi üzerinden erişim sağlanmıştır.

------------------------------------------------------------------------
5. YÜKSEK SKOR SİSTEMİ VE DOSYA FORMATI
------------------------------------------------------------------------
Yüksek skor sistemi için İkili Dosya (Binary File) formatı tercih edilmiştir.
Veriler "high_scores.dat" isimli bir dosyada saklanmaktadır.

- Yazma Modu: "wb" (Write Binary)
- Okuma Modu: "rb" (Read Binary)

Kullanılan Yöntem:
`game.c` dosyasındaki `Game_SaveHighScores` ve `Game_LoadHighScores` fonksiyonları,
`HighScore` yapısındaki (struct) veriyi `fwrite` ve `fread` fonksiyonları ile doğrudan
bellek bloğu olarak dosyaya yazar ve okur. 
Metin tabanlı değildir, bu sayede veriler dışarıdan kolayca değiştirilemez ve
okuma/yazma işlemi daha hızlıdır.


------------------------------------------------------------------------
6. EKİP GÖREV PAYLAŞIMI
------------------------------------------------------------------------

Projenin tamamı (oyun motoru mimarisi, fizik motoru, yapay zeka, UI tasarımı ve varlık yönetimi)
Eren Semih YILDIZ tarafından kodlanmıştır.
