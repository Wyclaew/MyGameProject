
İsim : Eren Semih

Soy isim : Yıldız

Öğrenci Numarası : 25253013

oyun fikri : Roguelite Survivors	

Oyun main.c üzerinde derlenip main.exe ile çalıştırılacak.

Zorunlu pointerı ateş etme fonksiyonunda ve çarpışma kontrolcüsünde kullandım, fonksiyonların içinde
kullandığım dizilerin başlangıç adreslerini fonksiyona göndermek için kullandım. Fonksiyonda o diziye
gidip asıl diziyi değiştirdi.


Yüksek skor sisteminde her satırda 1 isim ve 1 tane de skor için alan var eğer oyunucunun yaptığı skor
top 10 listesindeki diğer skorlardan fazlaysa hangi sıraya yerleşmesi gerek otomatik hesaplanıp oraya yerleştiriliyor,
ayrıca oyuncu yüksek skor yaptığında game over ekranında ismini girmesi için 20 karakterlik bir dizi tanımlı bu
şekilde ismini de giriyor ve bu iki değişken gamedata strutctı içinde tutulup sonrasında yüksek skorların bulunduğu
dosyaya yazdırılıp kayıt ediliyor, her oyunun sonunda yapılan skor kontrol edilip bu döngüye girip girmeyeceği
kontrol ediliyor.


Araştırmanın kodlamanın ve diğer her şeyin tamamını kendim yaptım.