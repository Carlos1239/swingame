program SpriteRotationTest;
uses SwinGame, sgTypes;

function SpriteLocationMatrix(s: Sprite): Matrix2D;
var
	scale, newX, newY, w, h: Single;
	anchorMatrix: Matrix2D;
begin
	scale := SpriteScale(s);
	w := SpriteLayerWidth(s, 0);
	h := SpriteLayerHeight(s, 0);

	anchorMatrix := TranslationMatrix(SpriteAnchorPoint(s));

	result := IdentityMatrix();
	result := MatrixMultiply(MatrixInverse(anchorMatrix), result);
	result := MatrixMultiply(RotationMatrix(SpriteRotation(s)), result);
	result := MatrixMultiply(anchorMatrix, result);

	newX := SpriteX(s) - (w * scale / 2.0) + (w / 2.0);
	newY := SpriteY(s) - (h * scale / 2.0) + (h / 2.0);
	result := MatrixMultiply(TranslationMatrix(newX / scale, newY / scale), result);

	result := MatrixMultiply(ScaleMatrix(scale), result);
end;

procedure Main();
var
	sprt, s2: Sprite;
	tri, initTri: Triangle;
	triB, initTriB: Triangle;
	r: Rectangle;
	q: Quad;
begin
	OpenWindow('Sprite Rotation', 600, 600);
	OpenWindow('Other Window', 300, 300);

	sprt := CreateSprite(BitmapNamed('rocket_sprt.png'));
	SpriteSetMoveFromAnchorPoint(sprt, true);
	SpriteSetX(sprt, 300);
	SpriteSetY(sprt, 300);

	s2 := CreateSprite(BitmapNamed('rocket_sprt.png'));
	SpriteSetMoveFromAnchorPoint(s2, true);
	SpriteSetX(s2, 100);
	SpriteSetY(s2, 100);

	r := RectangleFrom(400, 100, 100, 50);
	q := QuadFrom(r);
	ApplyMatrix(
		MatrixMultiply(TranslationMatrix(0, 50), RotationMatrix(45)) , q);

	initTri := TriangleFrom(0, 0, BitmapWidth(BitmapNamed('rocket_sprt.png')), BitmapHeight(BitmapNamed('rocket_sprt.png')), 0, BitmapHeight(BitmapNamed('rocket_sprt.png')));
	initTriB := TriangleFrom(BitmapWidth(BitmapNamed('rocket_sprt.png')), 0, BitmapWidth(BitmapNamed('rocket_sprt.png')), BitmapHeight(BitmapNamed('rocket_sprt.png')), 0, 0);

	repeat
		ProcessEvents();

		ClearScreen(ColorWhite);
		DrawFramerate(0, 0);

		if KeyTyped(OKey) then SetCurrentWindow('Other Window');
		if KeyTyped(MKey) then SetCurrentWindow('Sprite Rotation');

		if KeyDown(LeftKey) then
		begin
			SpriteSetRotation(sprt, SpriteRotation(sprt) - 5);
		end;

		if KeyDown(RightKey) then
		begin
			SpriteSetRotation(sprt, SpriteRotation(sprt) + 5);
		end;

		if KeyDown(ShiftKey) then
		begin
			if KeyDown(UpKey) then
			begin
				SpriteSetScale(sprt, SpriteScale(sprt) + 0.1);
			end;

			if KeyDown(DownKey) then
			begin
				SpriteSetScale(sprt, SpriteScale(sprt) - 0.1);
			end;
		end
		else
		begin
			if KeyDown(UpKey) then
			begin
				SpriteSetDY(sprt, SpriteDY(sprt) - 0.1);
			end;

			if KeyDown(DownKey) then
			begin
				SpriteSetDY(sprt, SpriteDY(sprt) + 0.1);
			end;
		end;

		if KeyTyped(Num0Key) then SpriteSetRotation(sprt, 0);
		if KeyTyped(Num9Key) then SpriteSetRotation(sprt, 45);

		tri := initTri;
		triB := initTriB;
		ApplyMatrix(SpriteLocationMatrix(sprt), tri);
		ApplyMatrix(SpriteLocationMatrix(sprt), triB);
		FillTriangle(ColorGreen, tri);
		FillTriangle(ColorGreen, triB);

		tri := initTri;
		triB := initTriB;
		ApplyMatrix(SpriteLocationMatrix(s2), tri);
		ApplyMatrix(SpriteLocationMatrix(s2), triB);
		FillTriangle(ColorBlue, tri);
		FillTriangle(ColorBlue, triB);
		// FillQuad(RGBAColor(0,0,255,62), q);

		if SpriteRectCollision(sprt, r) then
			FillRectangle(ColorPink, r)
		else
			DrawRectangle(ColorPurple, r);

		DrawBitmap('ufo.png', 400, 300);

		if SpriteBitmapCollision(sprt, BitmapNamed('ufo.png'), 400, 300) then
			DrawRectangle(ColorPurple, 400, 300, BitmapWidth(BitmapNamed('ufo.png')), BitmapHeight(BitmapNamed('ufo.png')));

		if SpriteAtPoint(sprt, MousePosition()) then
			FillCircle(ColorYellow, SpriteCollisionCircle(sprt));

		DrawSprite(sprt);
		DrawSprite(s2);
		UpdateSprite(sprt);

		if SpriteCollision(sprt, s2) then
		begin
			DrawCircle(ColorRed, SpriteCollisionCircle(s2));
		end;

		DrawCircle(ColorGreen, SpriteCollisionCircle(sprt));
		DrawRectangle(ColorGreen, SpriteCollisionRectangle(sprt));

		DrawLine(ColorGreen, LineFromVector(CenterPoint(sprt), MatrixMultiply(RotationMatrix(SpriteRotation(sprt)), VectorMultiply(SpriteVelocity(sprt), 10.0))));

		RefreshScreen();
	until WindowCloseRequested();

end;

begin
	Main();
end.
