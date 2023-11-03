'use strict';

const np_fs = require('fs');

exports.GetPage = function () {
    return '<tr><td></td><td>Load avg.:</td><td id="cpu_loadavg"></td></tr>' +
        '<tr><td id="cpu_thermal_status" bgcolor="white"></td><td>CPU thermal:</td><td id="cpu_thermal"></td></tr>';
}

exports.GetPageData = function (a_config) {
    let data = {};
    data.cpu_loadavg = GetText(a_config.loadavg);
    const thermal = (GetThermal(a_config.cpu_thermal) / a_config.cpu_thermaldiv).toFixed(2);
    data.cpu_thermal = thermal + ' \xB0C'; // 0xB0 - celsius degree sign
    data.cpu_thermal_status = GetThermalStatus(thermal);
    return data;
}

function GetThermal(a_fileName) {
    try {
        return np_fs.readFileSync(a_fileName, 'utf-8');
    }
    catch (err) {
        console.error(err);
    }
    return 0;
}

let g_cpu_thermal_status = 'gray';
function GetThermalStatus(a_value) {
    a_value = Math.floor(a_value);
    function CheckTemp(val, min, max) {
        const hysteresis = g_cpu_thermal_status != 'gray' ? 1 : 0;
        return val >= min + hysteresis && val <= max - hysteresis; // hysteresis
    }
    function CheckColour(val, min, max, colour) {
        if (!CheckTemp(val, min, max))
            return false;
        g_cpu_thermal_status = colour;
        return true;
    }
    if (CheckColour(a_value, 0, 30, 'blue'))
        return g_cpu_thermal_status;
    if (CheckColour(a_value, 30, 40, 'green'))
        return g_cpu_thermal_status;
    if (CheckColour(a_value, 40, 50, 'yellow'))
        return g_cpu_thermal_status;
    if (CheckColour(a_value, 50, 70, 'orange'))
        return g_cpu_thermal_status;
    if (CheckColour(a_value, 70, 100, 'red'))
        return g_cpu_thermal_status;
    return g_cpu_thermal_status;
}

function GetText(a_fileName) {
    try {
        let dataRaw = np_fs.readFileSync(a_fileName, 'utf-8');
        // Some strings contains trailing '\n' (/proc/loadavg for example) and it's not shown in the debugger.
        // Thus '\n' will be in JSON made from data containing this string.
        dataRaw = dataRaw.trimEnd();
        return dataRaw;
    }
    catch (err) {
        console.error(err);
    }
}