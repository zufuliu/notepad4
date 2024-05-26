images:
	Visual Studio Image Library 2017, 2022
	https://www.microsoft.com/en-us/download/details.aspx?id=35825

	https://learn.microsoft.com/en-us/windows/win32/uxguide/vis-icons
	https://learn.microsoft.com/en-us/windows/apps/design/style/iconography/app-icon-construction#icon-scaling

	res/Encoding.bmp	images/Encoding.svg, EncodingGray.svg
		Top: Cyrillic letter Ya, я or Я
		Bottom left: Latin small letter a
		Bottom right: Chinese character 宇 (universal)

	res/OpenFolder.bmp	images/Open.svg
	matepath/res/OpenFolder.bmp
	matepath/res/Goto.ico

	res/Run.ico			images/Launch.svg
	matepath/res/Run.ico

	16 color bitmap:
		res/Prev.bmp		Prev.svg
		res/Next.bmp		Next.svg

256.png
	https://github.com/paintdotnet/release
	https://lib.rs/crates/pngquant, https://github.com/kornelski/pngquant
	https://lib.rs/crates/oxipng, https://github.com/shssoichiro/oxipng

	pngquant --force --verbose 256 --strip --output Notepad4-quant256.png Notepad4.png
	save Notepad4-quant256.png as Notepad4-q256.png with Paint.NET
	oxipng --strip all --nc --out Notepad4-256.png Notepad4-q256.png

	pngquant --force --verbose 256 --strip --output matepath-quant256.png matepath.png
	save matepath-quant256.png as matepath-q256.png with Paint.NET
	oxipng --strip all --nc --out matepath-256.png matepath-q256.png
