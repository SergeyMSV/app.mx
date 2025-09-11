'use strict';

const np_fs = require('fs');

const sc_config = require('./config.js');
const sc_status = require('./status.js');

const g_confMXGNSS = sc_config.GetConfigMXGNSS();
const g_isSupportedMXGNSS = g_confMXGNSS !== undefined;

exports.GetPage = function () {
    let str =  '<tr><td id="status_color" bgcolor="white"></td><td>GNSS</td><td id="status_text"></td></tr>';
    if (g_isSupportedMXGNSS) {
        str += '<tr><td></td><td> - mode:</td><td id="mode_text"></td></tr>';
        str += '<tr><td></td><td> - UTC:</td><td id="utc_text"></td></tr>';
        str += '<tr><td></td><td> - location:</td><td id="location_text"></td></tr>';
    }
    return str;
}

exports.GetPageData = function () {

    if (!g_isSupportedMXGNSS)
        return { status: sc_status.GNSS.NotSupported };

    let filesGNSS = [];
    try {
        let filesAll = np_fs.readdirSync(g_confMXGNSS.out.path);
        for (let i = 0; i < filesAll.length; ++i) {
            if (filesAll[i].includes(g_confMXGNSS.out.prefix, 0)) {
                filesGNSS.push(filesAll[i]);
            }
        }
    } catch { }

    if (!filesGNSS.length)
        return { status: sc_status.GNSS.NoData, mode: '' };

    filesGNSS.sort();

    const fileLast = filesGNSS[filesGNSS.length - 1];
    const filename = fileLast.split('.')[0];
    const filenameParts = filename.split('_');

    if (filenameParts.length != 3)
        return { status: sc_status.GNSS.FileNameError };

    const timeStr = filenameParts[2].split('-').join(':');
    const fileLastDT = filenameParts[1] + ' ' + timeStr;

    let data = { };

    const filePath = g_confMXGNSS.out.path +'/' + fileLast;
    try {
        let gnssData = np_fs.readFileSync(filePath, 'utf-8');
        gnssData = JSON.parse(gnssData);

        data.mode = gnssData.mode_indicator.toLowerCase();
        data.utc = gnssData.utc;
        data.location = gnssData.latitude.toFixed(4) + ' ' + gnssData.longitude.toFixed(4);


        if (CheckGNSSTime(gnssData.utc)) {
            data.status = Object.assign({}, gnssData.valid == 'A' ? sc_status.GNSS.Valid : sc_status.GNSS.Invalid); // copying the object
        }
        else {
            data.status = Object.assign({}, sc_status.GNSS.NoData); // copying the object
        }

        data.status.text = fileLastDT; // [TBD] maybe somewhat else...

    } catch {
        return { status: sc_status.GNSS.FormatError };
    }

    return data;
}

function CheckGNSSTime(a_utc) {
    const utcDateTime = a_utc.split(' ');
    if (utcDateTime.length != 2)
        return false;
    const utcDate = utcDateTime[0].split('.');
    const utcTime = utcDateTime[1].split('.');
    if (utcDate.length != 3 || utcTime.length != 4)
        return false;
    const utcGNSS = Date.UTC('20' + utcDate[2], utcDate[1] - 1, utcDate[0], utcTime[0], utcTime[1], utcTime[2], utcTime[3]);

    const maxPeriod = g_confMXGNSS.NMEA.PeriodMAX_us / 1000; // converted to ms from us

    return Math.abs(Date.now() - utcGNSS) <= maxPeriod;
}