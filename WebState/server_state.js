'use strict';

const np_fs = require('fs');
const sc_utils = require('./utils.js');

exports.GetUptime = function (a_config) {
    let val = GetText(a_config);
    val = val.split(' ');
    if (val.length != 2)
        return '-';
    return sc_utils.PeriodToString(val[0] * 1000);
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
