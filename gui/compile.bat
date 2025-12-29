set SFML_PATH=C:\SFML

@REM g++ main.cpp ^
@REM  -I%SFML_PATH%\include ^
@REM  -L%SFML_PATH%\lib ^
@REM  -lsfml-graphics -lsfml-window -lsfml-system ^
@REM  -std=c++23 ^
@REM  -o game.exe

 g++ -g main.cpp board.cpp popup.cpp -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -std=c++23 -o game.exe
