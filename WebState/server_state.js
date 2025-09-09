'use strict';

const np_fs = require('fs');

exports.GetDateTime = function () {
    let dt = (new Date(Date.now())).toISOString();
    dt = dt.replace('T', ' ');
    dt = dt.replace('Z', '');
    return dt;
}

exports.GetUptime = function (a_config) {
    let val = GetText(a_config);
    val = val.split(' ');
    if (val.length != 2)
        return '-';
    const ut = GetUptime(val[0]);
    const str = ut.day + ' days ' + // format: "85 days 05:10:18"
        ut.hour.toString(10).padStart(2, '0') + ':' + // ECMAScript 2017
        ut.minute.toString(10).padStart(2, '0') + ':' +
        ut.second.toString(10).padStart(2, '0');
    return str;
}

function GetText(a_fileName) {
    try {
        const dataRaw = np_fs.readFileSync(a_fileName, 'utf-8');
        return dataRaw;
    }
    catch (err) {
        return 'error'; //console.error(err);
    }
}

function GetUptime(a_value) {
    let ut = {};
    ut.day = Math.floor(a_value / 86400);
    let ut_rem = ut.day * 86400;
    ut.hour = Math.floor((a_value - ut_rem) / 3600);
    ut_rem += ut.hour * 3600;
    ut.minute = Math.floor((a_value - ut_rem) / 60);
    ut_rem += ut.minute * 60;
    ut.second = Math.floor((a_value - ut_rem));
    ut_rem += ut.second;
    ut.millisecond = (a_value - ut_rem);
    return ut;
}