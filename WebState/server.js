#!/usr/bin/env node

'use strict';

const PRODUICTION = process.env.NODE_ENV === 'production';

const g_port = process.env.PORT || 1337;

const np_fs = require('fs');
const np_express = require('express');
const np_favicon = require('express-favicon');

let app = np_express();
app.use(np_favicon(__dirname + '/public/favicon.ico'));

const sc_server_cpu = require('./server_cpu');
const sc_server_data = require('./server_data');

const g_config = (() => {
    try {
        let path_mx = 'mx.conf.json';
        if (PRODUICTION)
            path_mx = '/etc/' + path_mx;
        let conf_mx = np_fs.readFileSync(path_mx, 'utf-8');
        conf_mx = JSON.parse(conf_mx); // get platform id in order to select appropriate settings

        let conf = np_fs.readFileSync('server.conf.json', 'utf-8');
        conf = JSON.parse(conf);

        for (const i in conf) { // add new config items in accordance with the platform id
            const platformId = i.split('_').pop();
            if (platformId == conf_mx.platform.id) {
                let paramId = i.slice(0, i.length - platformId.length - 1); // gets first part of the string; '- 1' is for '_'
                conf[paramId] = conf[i];
            }
        }
        for (const i in conf) { // add prefixes to paths in accordance with the platform id
            let val = conf[i];
            if (!PRODUICTION && val != undefined && typeof (val) === 'string' && val.length > 0 && val[0] === '/') {
                conf[i] = 'test_root_fs' + val;
            }
        }
        return conf;
    }
    catch (err) {
        console.error(err);
    }
})();

const g_hostname = (() => {
    try {
        return np_fs.readFileSync(g_config.hostname, 'utf-8');
    }
    catch (err) {
        console.error(err);
    }
    return 'NONAME';
})();

function GetHeader() {
    return '<title>' + g_hostname + '</title>';
}

app.get('/', (req, res) => {

    if (req.query.content == 'data') {
        let data = {};
        data.cpu = sc_server_cpu.GetPageData(g_config);
        data.datetime = sc_server_data.GetDateTime();
        data.uptime = sc_server_data.GetUptime(g_config.uptime);
        data.update_period = 500; // ms
        res.status(200).json(data);
        return;
       }

    res.status(200);
    res.append('Content-Type', 'text/html; charset=UTF-8');
    res.append('Content-Script-Type', 'text/javascript');

    let HtmlHead = GetHeader();
    HtmlHead += '<link rel="stylesheet" href="css/content.css" type="text/css" />';
    //HtmlHead += '<script src="js/jquery-3.7.1.js" type="text/javascript"></script>';
    HtmlHead += '<script src="js/jquery-3.7.1.min.js" type="text/javascript"></script>';
    HtmlHead += '<script src="js/page.js" type="text/javascript"></script>';
    HtmlHead += '<script>$(document).ready(function () { update(); });</script>';

    let HtmlBodyMain = '<table>';
    HtmlBodyMain += `<tr><td width=1px></td><td>Host name:</td><td id="hostname">${g_hostname}</td></tr>`;
    HtmlBodyMain += '<tr><td width=1px></td><td>Date:</td><td id="datetime"></td></tr>';
    HtmlBodyMain += '<tr><td></td><td>Uptime:</td><td id="uptime"></td></tr>';
    HtmlBodyMain += sc_server_cpu.GetPage();
    HtmlBodyMain += '</table>';

    const htmlDoc = `<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
${HtmlHead}
</head>
<body>
${HtmlBodyMain}
</body>
</html>`;

    res.end(htmlDoc);
});

app.get('/css/content.css', (req, res) => {
    np_fs.readFile(__dirname + '/public' + req.url, (err, file) => {
        res.setHeader('Content-Type', 'text/css'); res.end(file);
    });
});

app.get([
    //'/js/jquery-3.7.1.js',
    '/js/jquery-3.7.1.min.js',
    '/js/page.js'],
    (req, res) => {
        np_fs.readFile(__dirname + '/public' + req.url, (err, file) => {
            res.setHeader('Content-Type', 'text/javascript');
            res.end(file);
        });
    });

app.get('/sound/no_answer_peep.mp3', (req, res) => {
    np_fs.readFile(__dirname + '/public' + req.url, (err, file) => {
        res.setHeader('Content-Type', 'audio/mpeg'); res.end(file);
    });
});

app.listen(g_port);
