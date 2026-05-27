Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [System.IO.Compression.ZipFile]::OpenRead("D:\C++_document\QT\BanG_Tetrix!\bang-tetrix\完整项目简历.docx")
$stream = $zip.GetEntry("word/document.xml").Open()
$reader = New-Object System.IO.StreamReader($stream)
$text = $reader.ReadToEnd()
$reader.Close()
$zip.Dispose()
$text -replace '<[^>]+>', ''
