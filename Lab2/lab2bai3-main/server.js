const express = require('express');
const multer = require('multer');
const path = require('path');
const bodyParser = require('body-parser');
const app = express();
const port = process.env.PORT || 3000;


app.use(bodyParser.json());
app.use(
  bodyParser.urlencoded({
    extended: true,
  }),
);
// Thư mục lưu trữ các bản cập nhật
const uploadDirectory = "./update";

// Cấu hình Multer để xử lý tệp tải lên
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        cb(null, uploadDirectory);
    },
    filename: (req, file, cb) => {
        cb(null, Date.now() + path.extname(file.originalname));
    },
});

const upload = multer({ storage: storage });

// Đảm bảo thư mục lưu trữ tồn tại
const fs = require('fs');
if (!fs.existsSync(uploadDirectory)) {
    fs.mkdirSync(uploadDirectory);
}

const updateFiles = fs.readdirSync(uploadDirectory);



// Trang chủ hiển thị danh sách các bản cập nhật
app.get('/', (req, res) => {
    res.send(`
        <h1>Danh sách các bản cập nhật</h1>
        <ul>
            ${updateFiles.map((file) => `<li>${file}</li>`).join('')}
        </ul>
        <h2>Tải lên một bản cập nhật mới</h2>
        <form action="/upload" method="post" enctype="multipart/form-data">
            <input type="file" name="update">
            <input type="submit" value="Tải lên">
            <button onclick="deleteFile()">Delete</button>
        </form>

        <h2> Override version and name </h2>
        <form action="/override" method="post">
            <input type="text" name="version" placeholder="Version">
            <input type="text" name="name" placeholder="Name">
            <input type="text" name="path" placeholder="Path">
            <input type="submit" value="Submit">
        </form>

        <script>
        // function deleteFile() {
        //   fetch('/delete/C:\\Users\\Lenovo\\Documents\\Arduino', {
        //     method: 'DELETE'
        //   })
        //   .then(response => response.text())
        //   .then(data => {
        //     console.log(data);
        //   })
        //   .catch(error => console.error('Error:', error));
        // }
       </script>
    `);
});

app.post('/override', (req, res) => {
  const newData = req.body;
  const config = JSON.parse(fs.readFileSync('./config.json', 'utf-8'));
  config.version = newData.version;
  config.name = newData.name;
  config.path = newData.path;
  fs.writeFileSync('./config.json', JSON.stringify(config), 'utf-8');
  res.redirect('/');
})

// Route xử lý tải lên tệp
app.post('/upload', upload.single('update'), (req, res) => {
    res.redirect('/');
});

// Xử lý delete file
app.delete('/delete/:filename', function (req, res, next) {
    const fileName = req.params.filename;
    const filePath = path.join(__dirname, 'uploads', fileName);
    fs.unlink(filePath, (err) => {
      if (err) {
        console.error(err);
        res.status(500).send('Error deleting file');
      } else {
        res.send('File deleted successfully');
      }
    });
});

// const updatesBinDirectory = path.join(__dirname, 'update'); // Đường dẫn đến thư mục "updates"
app.use('/',express.static(__dirname));

app.listen(port, () => {
    console.log(`Server đang chạy tại http://localhost:${port}`);
});
