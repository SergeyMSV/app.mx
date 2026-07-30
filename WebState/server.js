#!/usr/bin/env node

'use strict';

const version = '0.2.1';

const np_fs = require('fs');
const np_express = require('express');
const np_favicon = require('express-favicon');

let app = np_express();
app.use(np_favicon(__dirname + '/public/favicon.ico'));

const sc_config = require('./config.js');
const sc_page_cpu = require('./page_cpu.js');
const sc_page_hwmon = require('./page_hwmon.js');
const sc_page_gnss = require('./page_gnss.js');
const sc_utils = require('./utils.js');

app.get('/', (req, res) => {

    const conf = sc_config.GetConfig();
    if (conf === undefined)
        return;

    if (req.query.content == 'data') {
        let data = {};
        data.cpu = sc_page_cpu.GetPageData(conf);
        data.host = {};
        data.host.utc = sc_utils.DateToString(new Date);
        data.host.uptime = sc_utils.GetUptime();
        data.host.color = 'green'; // [TBD] it is to be more useful
        data.hwmon = sc_page_hwmon.GetPageData();
        //data.gnss = sc_page_gnss.GetPageData();
        data.update_period = 500; // ms
        res.status(200).json(data);
        return;
       }

    res.status(200);
    res.append('Content-Type', 'text/html; charset=UTF-8');
    res.append('Content-Script-Type', 'text/javascript');

    const hostname = sc_utils.GetHostname();

    const htmlDoc = `<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>${hostname}</title>
<link rel="stylesheet" href="css/content.css" type="text/css" />
<script src="js/jquery-3.7.1.min.js" type="text/javascript"></script>
<script src="js/page.js" type="text/javascript"></script>
<script>$(document).ready(function () { update(); });</script>
</head>
<body>
<table>
<tr><td id="host_color" width=1px></td><td>Host</td><td id="host_name">${hostname} (${version})</td></tr>
<tr><td width=1px></td><td> UTC</td><td id="host_utc"></td></tr>
<tr><td></td><td> uptime</td><td id="host_uptime"></td></tr>
${sc_page_cpu.GetPage()}
${sc_page_hwmon.GetPage()}
</table>
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

app.listen(process.env.PORT || 1337);
