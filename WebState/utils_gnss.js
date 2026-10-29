'use strict'

const sc_utils = require('./utils.js');

function GetSatQty(sats, min, max) {
    let qty = {};
    qty.all = 0;
    qty.used = 0;
    try {
        for (const i of sats) {
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

function AddMaxNumbers(arr, newNum, maxCount) {
    arr.push(newNum);
    arr.sort((a, b) => b - a);
    arr.length = maxCount;
    return arr;
}

function GetSatSnrs(sats, min, max, maxCount) {
    let snrs = [];
    for (const i of sats) {
        if (i.id >= min && i.id <= max && i.snr > 0)
            AddMaxNumbers(snrs, i.snr, maxCount);
    }
    return snrs;
}

function GetSatsStr(a_sats, a_min, a_max) {
    const qty = GetSatQty(a_sats, a_min, a_max);
    let qtyStr = qty.used.toString(10).padStart(2, '0') + '/' + qty.all.toString(10).padStart(2, '0');
    const maxCount = Math.min(6, qty.used);
    const snrs = GetSatSnrs(a_sats, a_min, a_max, maxCount);
    for (const i of snrs) {
        qtyStr += " " + i.toString(10).padStart(2, '0');
    }
    return qtyStr;
}

exports.GetSatsStrGPS = function (a_gnss) {
    return GetSatsStr(a_gnss.sats, 1, 32);
}

exports.GetSatsStrGLO = function (a_gnss) {
    return GetSatsStr(a_gnss.sats, 65, 96);
}

exports.GetSatsStrWAA = function (a_gnss) {
    return GetSatsStr(a_gnss.sats, 33, 64); // WAAS
}

exports.GetColor = function (a_gnss) {
    // "red" is for an error from an exception
    return a_gnss.valid === "A" ? "green" : "yellow"; // [TBD] - spoofing should be taken into consideration
}

exports.GetPosSpdShort = function (a_gnss) {
    return a_gnss.lat.toFixed(4) + " " + a_gnss.lon.toFixed(4).toString(10).padStart(8, ' ') + " " + a_gnss.spd.toFixed(0).toString(10).padStart(3, ' ');
}

function RetrieveUTC(a_gnssUTC) {
    const utcDateTime = a_gnssUTC.split(' ');
    if (utcDateTime.length != 2)
        throw new Error('format error');
    const utcDate = utcDateTime[0].split('-');
    const utcTime = utcDateTime[1].split(/[:.]/);
    if (utcDate.length != 3 || utcTime.length != 4)
        throw new Error('format error');
    return Date.UTC(utcDate[0], utcDate[1] - 1, utcDate[2], utcTime[0], utcTime[1], utcTime[2], utcTime[3]);
}

const g_gnssStatus = {
    None: { text: '', color: 'gray' },
    NotSupported: { text: 'not supported', color: 'gray' },
    NoData: { text: 'no data', color: 'orange' },
    FileNameError: { text: 'file name format error', color: 'orange' },
    FormatError: { text: 'data format error', color: 'orange' },
    TimeError: { text: 'time error', color: 'orange' }, // e.g. GPS spoofing
    Invalid: { text: 'not valid', color: 'yellow' },
    Valid: { text: '', color: 'green' },
}

exports.GetStatus = function (a_gnss, a_utcCapture) {
    const gnssValidityPeriod = 5000; // [#] 5 s
    const gnssSuspiciousPeriod = 1000; // [#] 1 s
    const utcNow = Date.now();
    const utcGNSS = RetrieveUTC(a_gnss.utc);

    // The file is outdated, the containing data is not interesting.
    if (Math.abs(utcNow - a_utcCapture) > gnssValidityPeriod)
        return Object.assign({}, g_gnssStatus.NoData); // copying the object

    // The file is not outdated but UTC in the containing data is not correct (e.g. GPS spoofing).
    if (Math.abs(utcNow - utcGNSS) > gnssValidityPeriod)
        return Object.assign({}, g_gnssStatus.NoData);

    if (Math.abs(a_utcCapture - utcGNSS) > gnssSuspiciousPeriod) { // if the difference between a_utcCapture and utcGNSS is more then 1 s - it doesn't seem to be all right.
        let status = Object.assign({}, g_gnssStatus.TimeError); // copying the object
        status.text += ': ' + sc_utils.PeriodToString(utcGNSS - a_utcCapture);
        status.class = 'text_orange';
        return status;
    }

    return Object.assign({}, gnssData.valid == 'A' ? g_gnssStatus.Valid : g_gnssStatus.Invalid);
}
