'use strict'

const np_fs = require('fs');

const PRODUICTION = process.env.NODE_ENV === 'production';

exports.DateToString = function (a_date) {
    let dt = a_date.toISOString();
    dt = dt.replace('T', ' ');
    dt = dt.replace('Z', '');
    return dt;
}

exports.PeriodToString = function (a_period) { // period is in ms
    const per = ParsePeriod(a_period);
    const str = (per.negative ? '- ' : '') + per.days + ' days ' + // format: "85 days 05:10:18"
        per.hours.toString(10).padStart(2, '0') + ':' + // ECMAScript 2017
        per.minutes.toString(10).padStart(2, '0') + ':' +
        per.seconds.toString(10).padStart(2, '0');// + '.' +
    //per.milliseconds.toString(10).padStart(3, '0');
    return str;
}

function ParsePeriod(a_period) { // period is in ms
    let per = {};
    per.negative = a_period < 0;
    if (per.negative)
        a_period = -a_period;
    per.days = Math.floor(a_period / 86400000);
    a_period -= per.days * 86400000;
    per.hours = Math.floor(a_period / 3600000);
    a_period -= per.hours * 3600000;
    per.minutes = Math.floor(a_period / 60000);
    a_period -= per.minutes * 60000;
    per.seconds = Math.floor(a_period / 1000);
    a_period -= per.seconds * 1000;
    per.milliseconds = Math.floor(a_period / 1000);
    return per;
}

exports.ParseFileName = function (a_fileName) {
    let temp = a_fileName.split('.');
    if (temp.length != 2)
        return undefined;
    let value = { ext: temp[1] };
    temp = temp[0].split('_');
    if (temp.length != 3)
        return undefined;
    value.prefix = temp[0];
    const timeStr = temp[2].split('-').join(':');
    let date = temp[1] + 'T' + timeStr + '.000Z';
    value.utc = Date.parse(date);
    value.dateTime = temp[1] + ' ' + timeStr;
    return value;
}

function GetRealPath(a_path) {
    if (!PRODUICTION && typeof (a_path) === 'string' && a_path[0] === '/')
        return 'test_root_fs' + a_path;
    return a_path;
}

exports.ReadDir = function (a_path) {
    try {
        return np_fs.readdirSync(GetRealPath(a_path));
    }
    catch (err) {
        console.error(err);
    }
    return [];
}

exports.ReadFile = function (a_path, a_default) {
    try {
        return np_fs.readFileSync(GetRealPath(a_path), 'utf-8');
    }
    catch (err) {
        console.error(err);
    }
    return a_default;
}

exports.ReadFileJSON = function (a_path) {
    try {
        const conf = np_fs.readFileSync(GetRealPath(a_path), 'utf-8');
        return JSON.parse(conf);
    }
    catch (err) {
        console.error(err);
    }
    return '';
}

exports.ReadConfig = function (a_configName, a_configDir) {
    try {
        const conf = np_fs.readFileSync(PRODUICTION ? a_configDir + '/' + a_configName : a_configName, 'utf-8');
        return JSON.parse(conf);
    }
    catch (err) {
        console.error(err);
    }
    return '';
}

exports.GetHostname = function () { return exports.ReadFile('/etc/hostname', 'NONAME'); }
exports.GetLoadAvg = function () { return exports.ReadFile('/proc/loadavg', ''); }
exports.GetLoadAvgShort = function () {
    const loadavg = exports.GetLoadAvg();
    for (let i = 0, space_counter = 0; i < loadavg.length; ++i) {
        //0.22 0.15 0.10 1/104 36525
        if (loadavg[i] === ' ' && ++space_counter === 4) {
            return loadavg.slice(0, i);
            }
    }
    return loadavg;
}
exports.GetUptime = function () {
    let val = exports.ReadFile('/proc/uptime', '');
    val = val.split(' ');
    if (val.length != 2)
        return '-';
    return exports.PeriodToString(val[0] * 1000);
}
