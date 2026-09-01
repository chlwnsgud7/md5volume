#include <bits/stdc++.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

struct MD5 {
    u32 st[4];
    static const u32 S[64], K[64];

    inline u32 rotl(u32 x, u32 n) { return (x << n) | (x >> (32 - n)); }

    void transform(const u8 b[64]) {
        u32 M[16];
        for (int i = 0; i < 16; i++)
            M[i] = b[i*4] | (b[i*4+1] << 8) | (b[i*4+2] << 16) | (b[i*4+3] << 24);

        u32 a = st[0], b_ = st[1], c = st[2], d = st[3];

        for (int i = 0; i < 64; i++) {
            u32 f, g;
            if (i < 16)      f = (b_ & c) | (~b_ & d), g = i;
            else if (i < 32) f = (d & b_) | (~d & c), g = (5*i + 1) & 15;
            else if (i < 48) f = b_ ^ c ^ d,          g = (3*i + 5) & 15;
            else             f = c ^ (b_ | ~d),       g = (7*i) & 15;

            u32 tmp = d; d = c; c = b_;
            b_ += rotl(a + f + K[i] + M[g], S[i]);
            a = tmp;
        }

        st[0] += a; st[1] += b_; st[2] += c; st[3] += d;
    }

    string digest(string s) {
        st[0] = 0x67452301; st[1] = 0xefcdab89;
        st[2] = 0x98badcfe; st[3] = 0x10325476;

        vector<u8> v(s.begin(), s.end());
        u64 bits = (u64)s.size() * 8;

        v.push_back(0x80);
        while ((v.size() & 63) != 56) v.push_back(0);
        for (int i = 0; i < 8; i++) v.push_back((bits >> (i * 8)) & 0xff);

        for (size_t i = 0; i < v.size(); i += 64) transform(&v[i]);

        char res[33];
        for (int i = 0; i < 4; i++) {
            sprintf(res + i * 8, "%02x%02x%02x%02x",
                    st[i] & 0xff, (st[i] >> 8) & 0xff,
                    (st[i] >> 16) & 0xff, (st[i] >> 24) & 0xff);
        }
        return string(res);
    }

    string operator()(string s) {
        return digest(s);
    }
};

const u32 MD5::S[64] = {
    7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
    5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
    4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
    6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
};

const u32 MD5::K[64] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};

const string HTML = R"HTML(
<!DOCTYPE html>
<html lang="ko">
<head>
<meta charset="UTF-8">
<title>MD5 볼륨 배틀</title>
<style>
  body { font-family: monospace; max-width: 640px; margin: 40px auto; }
  input { width: 100%; padding: 8px; font-family: monospace; }
  #hash { color: gray; word-break: break-all; }
  #vol  { font-size: 3em; font-weight: bold; }
</style>
</head>
<body>
<h2>🔊 MD5 볼륨 배틀</h2>
<input id="inp" type="text" placeholder="원하는 볼륨을 얻을 문자열을 찾아보세요">
<button onclick="go()">해시</button>
<p id="hash"></p>
<p id="last"></p>
<p>볼륨: <span id="vol">??</span></p>
<div id="player"></div>

<script>
// YouTube IFrame API 로드
var tag = document.createElement('script');
tag.src = "https://www.youtube.com/iframe_api";
document.head.appendChild(tag);

var player;
function onYouTubeIframeAPIReady() {
  player = new YT.Player('player', {
    height: '315', width: '560',
    videoId: 'dQw4w9WgXcQ',
    playerVars: { autoplay: 1, mute: 1 }  // autoplay는 mute 필요
  });
}

async function go() {
  const text = document.getElementById('inp').value;
  const res  = await fetch('/hash', {
    method: 'POST',
    headers: { 'Content-Type': 'text/plain' },
    body: text
  });
  const data = await res.json();

  document.getElementById('hash').textContent = data.hash;
  document.getElementById('last').textContent = "끝 두 자리: " + data.last + " / 255";
  document.getElementById('vol').textContent  = data.volume;

  if (player && player.setVolume) {
    player.unMute();
    player.setVolume(data.volume);
  }
}
</script>
</body>
</html>
)HTML";

string http_response(int code, const string& ct, const string& body) {
    string status = (code == 200) ? "200 OK" : "404 Not Found";
    return "HTTP/1.1 " + status + "\r\n"
           "Content-Type: "   + ct + "\r\n"
           "Content-Length: " + to_string(body.size()) + "\r\n"
           "Connection: close\r\n\r\n" + body;
}

void handle(int fd) {
    char buf[8192] = {};
    int n = recv(fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) { close(fd); return; }

    string req(buf, n);
    string method, path;
    istringstream(req) >> method >> path;

    string resp;
    if (method == "GET" && path == "/") {
        resp = http_response(200, "text/html; charset=utf-8", HTML);

    } else if (method == "POST" && path == "/hash") {
        auto sep  = req.find("\r\n\r\n");
        string body = (sep != string::npos) ? req.substr(sep + 4) : "";
        while (!body.empty() && (body.back() == '\r' || body.back() == '\n'))
            body.pop_back();

        MD5 md5;
        string hash = md5(body);

        int last = stoi(hash.substr(30, 2), nullptr, 16);
        int vol  = last * 100 / 255;

        char buf[512];
        sprintf(buf, "{\"hash\":\"%s\",\"volume\":%d,\"last\":%d}",
            hash.c_str(), vol, last);
        string json(buf);
        resp = http_response(200, "application/json", json);

    } else {
        resp = http_response(404, "text/plain", "Not Found");
    }

    send(fd, resp.c_str(), resp.size(), 0);
    close(fd);
}

int main() {
    #ifdef ONLINE_JUDGE
    ios::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    #endif

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(8080);

    bind(sfd, (sockaddr*)&addr, sizeof(addr));
    listen(sfd, 16);
    cerr << "http://localhost:8080\n";

    while (true) {
        int cfd = accept(sfd, nullptr, nullptr);
        thread([cfd]{ handle(cfd); }).detach();
    }
}