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
	metapath/res/OpenFolder.bmp
	metapath/res/Goto.ico

	res/Run.ico			images/Launch.svg
	metapath/res/Run.ico

	16 color bitmap:
		res/Prev.bmp		Prev.svg
		res/Next.bmp		Next.svg

256.png
	https://github.com/paintdotnet/release
	https://lib.rs/crates/pngquant, https://github.com/kornelski/pngquant
	https://lib.rs/crates/oxipng, https://github.com/shssoichiro/oxipng

	pngquant --force --verbose 256 --strip --output Notepad2-quant256.png Notepad2.png
	save Notepad2-quant256.png as Notepad2-q256.png with Paint.NET
	oxipng --strip all --nc --out Notepad2-256.png Notepad2-q256.png

	pngquant --force --verbose 256 --strip --output metapath-quant256.png metapath.png
	save metapath-quant256.png as metapath-q256.png with Paint.NET
	oxipng --strip all --nc --out metapath-256.png metapath-q256.png
