program BitmapDrawingTest;
uses SwinGame;

procedure Main();
begin
	PlayMusic('Fast.mp3');
	OpenGraphicsWindow('BitmapDrawingTest', 600, 600);
	LoadDefaultColors();

	repeat
		ProcessEvents();

		ClearScreen(ColorBlack);
		DrawBitmap('bubble.png', 50, 50);
		RefreshScreen();

	until WindowCloseRequested();
end;

begin
	Main();
end.