'use strict';

const np_fs = require('fs');

const sc_config = require('./config.js');
const sc_status = require('./status.js');
const sc_utils = require('./utils.js')

const g_confMXGNSS = sc_config.GetConfigMXGNSS();
const g_isSupportedMXGNSS = g_confMXGNSS !== undefined;

exports.GetPage = function () {
    let str =  '<tr><td id="gnss_status_color" bgcolor="white"></td><td>GNSS</td><td id="gnss_status_text"></td></tr>';
    if (g_isSupportedMXGNSS) {
        str += '<tr><td></td><td> - UTC fixed:</td><td id="gnss_utc_file_text"></td></tr>';
        str += '<tr><td></td><td> - UTC:</td><td id="gnss_utc_text"></td></tr>';
        str += '<tr><td></td><td> - mode:</td><td id="gnss_mode_text"></td></tr>';
        str += '<tr><td></td><td> - satellites:</td><td><span id="gnss_sats_gps"></span> <span id="gnss_sats_glo"></span></td></tr>';
        str += '<tr><td></td><td> - location:</td><td id="gnss_location_text"></td></tr>';
        str += '<tr><td></td><td> - speed:</td><td id="gnss_speed_text"></td></tr>';
    }
    return str;
}

exports.GetPageData = function () {

    if (!g_isSupportedMXGNSS)
        return { status: sc_status.GNSS.NotSupported };

    let filesGNSS = [];
    try {
        let filesAll = np_fs.readdirSync(g_confMXGNSS.out.path);
        for (const i of filesAll) {
            if (i.includes(g_confMXGNSS.out.prefix, 0)) {
                filesGNSS.push(i);
            }
        }
    } catch { }

    if (!filesGNSS.length)
        return { status: sc_status.GNSS.NoData, mode: '' };

    filesGNSS.sort();

    const fileLast = filesGNSS[filesGNSS.length - 1];

    const fileNameParsed = sc_utils.ParseFileName(fileLast);
    if (fileNameParsed === undefined)
        return { status: sc_status.GNSS.FileNameError };

    let data = {};

    const filePath = g_confMXGNSS.out.path + '/' + fileLast;
    try {
        let gnssData = np_fs.readFileSync(filePath, 'utf-8');
        gnssData = JSON.parse(gnssData);

        data.mode = gnssData.mode_indicator.toLowerCase();
        data.sats = GetSatStatus(gnssData);
        data.location = gnssData.latitude.toFixed(4) + ' ' + gnssData.longitude.toFixed(4);
        data.speed = gnssData.speed.toFixed(0) + ' km/h'

        const utcGNSS = GNSS_UTC_ToDT(gnssData.utc);
        const utcNow = Date.now();
        const utcFile = fileNameParsed.utc;

        data.utc = sc_utils.DateToString(new Date(utcGNSS));

        if (Math.abs(utcNow - utcFile) > g_confMXGNSS.NMEA.PeriodMAX_ms) { // The file is outdated, the containing data is not interesting.
            data.status = Object.assign({}, sc_status.GNSS.NoData); // copying the object
            data.status.class = 'text_red';
        }
        // The file is not outdated but UTC in the containing data is not correct (e.g. GPS spoofing).
        //else if (Math.abs(utcNow - utcGNSS) > g_confMXGNSS.NMEA.PeriodMAX_ms) {
        //    data.status = Object.assign({}, sc_status.GNSS.NoData); // copying the object --- ???
        //}
        else if (Math.abs(utcFile - utcGNSS) > 1000) { // if the difference between utcFile and utcGNSS is more then 1 s - it doesn't seem to be all right.
            data.status = Object.assign({}, sc_status.GNSS.TimeError); // copying the object
            data.status.text += ': ' + sc_utils.PeriodToString(utcGNSS - utcFile);
            data.status.class = 'text_orange';
        }
        else {
            data.status = Object.assign({}, gnssData.valid == 'A' ? sc_status.GNSS.Valid : sc_status.GNSS.Invalid); // copying the object
        }
    } catch (err) {
        data = {};
        data.status = sc_status.GNSS.FormatError;
    }

    data.utc_file = fileNameParsed.dateTime;

    return data;
}

function GNSS_UTC_ToDT(a_nmeaUTC) {
    const utcDateTime = a_nmeaUTC.split(' ');
    if (utcDateTime.length != 2)
        throw new Error('format error');
    const utcDate = utcDateTime[0].split('.');
    const utcTime = utcDateTime[1].split('.');
    if (utcDate.length != 3 || utcTime.length != 4)
        throw new Error('format error');
    return Date.UTC('20' + utcDate[2], utcDate[1] - 1, utcDate[0], utcTime[0], utcTime[1], utcTime[2], utcTime[3]);
}

function GetSatStatus(a_gnssData) {

    function GetSatQty(gnssData, min, max) {
        let qty = {};
        qty.all = 0;
        qty.used = 0;
        try {           
            for (const i of gnssData.satellite) {
                if (i.id >= min && i.id <= max) {
                    ++qty.all;
                    if (i.snr > 0)
                        ++qty.used;
                }
            }
        }
        catch { }
        return qty;
    }

    function SatQtyToString(label, satQty) {
        return label + ':' + satQty.used.toString(10).padStart(2, '0') + '(' + satQty.all.toString(10).padStart(2, '0') + ')';
    }

    function GetSatsClass(a_qty) {
        if (a_qty.all == 0 || a_qty.used == 0)
            return 'text_red';
        if (a_qty.used <= 2)
            return 'text_orange';
        if (a_qty.used <= 3)
            return 'text_yellow';
        return undefined; // default class
    }

    const gpsQty = GetSatQty(a_gnssData, 1, 32);
    const gloQty = GetSatQty(a_gnssData, 65, 96);
    //const waaQty = GetSatQty(a_gnssData, 33, 64); // WAAS

    let sats = {};
    sats.gps = {};
    sats.glo = {};
    
    sats.gps.text = SatQtyToString('GPS', gpsQty);
    sats.gps.class = GetSatsClass(gpsQty);
    
    sats.glo.text = SatQtyToString('GLO', gloQty);
    sats.glo.class = GetSatsClass(gloQty);

    return sats;
}
