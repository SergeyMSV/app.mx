'use strict';

const sc_config = require('./config.js');
//const sc_utils_status = require('./status.js');
const sc_utils = require('./utils.js')
const sc_utils_gnss = require('./utils_gnss.js')

exports.GetPage = function () {
    return MakePart(null) + MakePart("1") + MakePart("2");
}

exports.GetPageData = function () {
    let gnssmon = {};
    let data = MakePageData(null);
    if (data !== null)
        gnssmon.gnss = data;
    data = MakePageData("1");
    if (data !== null)
        gnssmon.gnss1 = data;
    data = MakePageData("2");
    if (data !== null)
        gnssmon.gnss2 = data;
    return gnssmon;
}

function MakePart(a_id) {
    const data = MakePageData(a_id);
    if (data === null)
        return "";
    const idStr = "gnss" + (a_id ?? "");
    const str = `<tr><td id="color${idStr}" bgcolor="white"></td><td>GNSS ${a_id ?? ""}</td><td id="input${idStr}_mode"></td></tr>`;
    return str + `
<tr><td></td><td> UTC</td><td id="input${idStr}_utc"></td></tr>
<tr><td></td><td> GPS</td><td id="input${idStr}_gps"></td></tr>
<tr><td></td><td> GLO</td><td id="input${idStr}_glo"></td></tr>
`;
}

function MakePageData(a_id) {
    const idStr = "gnss" + (a_id ?? "");
    const appName = "mx" + idStr;
    const configOut = sc_config.GetConfig().out;
    const fileLatest = sc_utils.GetOutFile(configOut.path, appName);
    if (fileLatest === undefined || !fileLatest.length)
        return null;
    const fileNameParsed = sc_utils.ParseFileName(fileLatest);
    if (fileNameParsed === undefined)
        return null;
    let data = {};
    const filePath = configOut.path + '/' + fileLatest;
    try {
        const gnss = sc_utils.ReadFileJSON(filePath);
        data.input = {};
        data.input[idStr + "_mode"] = sc_utils_gnss.GetPosSpdShort(gnss);//gnss.mode.toLowerCase();
        data.input[idStr + "_utc"] = gnss.utc;
        data.input[idStr + "_gps"] = sc_utils_gnss.GetSatsStrGPS(gnss);
        data.input[idStr + "_glo"] = sc_utils_gnss.GetSatsStrGLO(gnss);

        data.test = sc_utils_gnss.GetStatus(gnss, fileNameParsed.utc); // [TBD][TEST]

        data.color = {}
        data.color[idStr] = sc_utils_gnss.GetColor(gnss)
    } catch (err) {
        data = {};
        data.color = {}
        data.color[idStr] = "red";
    }

    /*
    const filePath = g_confMXGNSS.out.path + '/' + fileLast;
    try {
        const gnssData = sc_utils.ReadFileJSON(filePath);

        data.mode = gnssData.mode_indicator.toLowerCase();
        data.sats = GetSatStatus(gnssData);
        data.location = gnssData.latitude.toFixed(4) + ' ' + gnssData.longitude.toFixed(4);
        data.speed = gnssData.speed.toFixed(0) + ' km/h'

        const utcGNSS = GNSS_UTC_ToDT(gnssData.utc);
        const utcNow = Date.now();
        const utcFile = fileNameParsed.utc;

        data.utc = sc_utils.DateToString(new Date(utcGNSS));

        if (Math.abs(utcNow - utcFile) > g_confMXGNSS.NaviValidityPeriod) { // The file is outdated, the containing data is not interesting.
            data.status = Object.assign({}, sc_utils_status.GNSS.NoData); // copying the object
            data.status.class = 'text_red';
        }
        // The file is not outdated but UTC in the containing data is not correct (e.g. GPS spoofing).
        //else if (Math.abs(utcNow - utcGNSS) > g_confMXGNSS.NaviValidityPeriod) {
        //    data.status = Object.assign({}, sc_utils_status.GNSS.NoData); // copying the object --- ???
        //}
        else if (Math.abs(utcFile - utcGNSS) > 1000) { // if the difference between utcFile and utcGNSS is more then 1 s - it doesn't seem to be all right.
            data.status = Object.assign({}, sc_utils_status.GNSS.TimeError); // copying the object
            data.status.text += ': ' + sc_utils.PeriodToString(utcGNSS - utcFile);
            data.status.class = 'text_orange';
        }
        else {
            data.status = Object.assign({}, gnssData.valid == 'A' ? sc_utils_status.GNSS.Valid : sc_utils_status.GNSS.Invalid); // copying the object
        }
    } catch (err) {
        data = {};
        data.status = sc_utils_status.GNSS.FormatError;
    }
    */
    data.utc_file = fileNameParsed.dateTime;
    return data;
}

//function GNSS_UTC_ToDT(a_nmeaUTC) {
//    const utcDateTime = a_nmeaUTC.split(' ');
//    if (utcDateTime.length != 2)
//        throw new Error('format error');
//    const utcDate = utcDateTime[0].split('.');
//    const utcTime = utcDateTime[1].split('.');
//    if (utcDate.length != 3 || utcTime.length != 4)
//        throw new Error('format error');
//    return Date.UTC('20' + utcDate[2], utcDate[1] - 1, utcDate[0], utcTime[0], utcTime[1], utcTime[2], utcTime[3]);
//}



/*
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
*/
