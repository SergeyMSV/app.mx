#!/usr/bin/env node

'use strict';

const np_fs = require('fs');
const np_express = require('express');
const np_favicon = require('express-favicon');

let app = np_express();
app.use(np_favicon(__dirname + '/public/favicon.ico'));

const sc_config = require('./config.js');
const sc_server_cpu = require('./server_cpu.js');
const sc_server_state = require('./server_state.js');

const g_conf = sc_config.GetConfig(); // [TBD] do it in a proper way
const g_hostname = sc_config.GetHostname(); // [TBD] do it in a proper way

app.get('/', (req, res) => {

    if (g_conf === undefined)
        return;

    if (req.query.content == 'data') {
        let data = {};
        data.cpu = sc_server_cpu.GetPageData(g_conf);
        data.datetime = sc_server_state.GetDateTime();
        data.uptime = sc_server_state.GetUptime(g_conf.uptime);
        data.update_period = 500; // ms
        res.status(200).json(data);
        return;
       }

    res.status(200);
    res.append('Content-Type', 'text/html; charset=UTF-8');
    res.append('Content-Script-Type', 'text/javascript');

    let HtmlHead = '<title>' + g_hostname + '</title>';
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

app.listen(sc_config.PORT);
