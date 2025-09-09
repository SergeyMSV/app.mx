'use strict';

const np_fs = require('fs');

const sc_status = require('./status.js');

exports.GetPage = function () {
    return '<tr><td></td><td>Load avg.:</td><td id="cpu_loadavg"></td></tr>' +
        '<tr><td id="cpu_thermal_status" bgcolor="white"></td><td>CPU thermal:</td><td id="cpu_thermal"></td></tr>';
}

exports.GetPageData = function (a_config) {
    let data = {};
    data.cpu_loadavg = GetText(a_config.loadavg.path);
    const thermal = (GetThermal(a_config.cpu_thermal.path) / a_config.cpu_thermaldiv).toFixed(2);
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

let g_cpu_thermal_status = sc_status.CPU.Color.None
function GetThermalStatus(a_value) {
    a_value = Math.floor(a_value);
    function CheckTemp(val, min, max) {
        const hysteresis = g_cpu_thermal_status != sc_status.CPU.Color.None ? 1 : 0;
        return val >= min + hysteresis && val <= max - hysteresis; // hysteresis
    }
    function CheckColor(val, min, max, color) {
        if (!CheckTemp(val, min, max))
            return false;
        g_cpu_thermal_status = color;
        return true;
    }
    if (CheckColor(a_value, 0, 30, sc_status.CPU.Color.Cold))
        return g_cpu_thermal_status;
    if (CheckColor(a_value, 30, 40, sc_status.CPU.Color.Normal))
        return g_cpu_thermal_status;
    if (CheckColor(a_value, 40, 50, sc_status.CPU.Color.Warm))
        return g_cpu_thermal_status;
    if (CheckColor(a_value, 50, 70, sc_status.CPU.Color.Hot))
        return g_cpu_thermal_status;
    if (CheckColor(a_value, 70, 100, sc_status.CPU.Color.Critical))
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
        return 'error'; // console.error(err);
    }
}