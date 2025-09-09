'use strict';

const np_fs = require('fs');

const sc_status = require('./status.js');

exports.GetPage = function () {
    return '<tr><td></td><td>Load avg.:</td><td id="cpu_loadavg"></td></tr>' +
        '<tr><td id="cpu_thermal_color" bgcolor="white"></td><td>CPU thermal:</td><td id="cpu_thermal_text"></td></tr>';
}

exports.GetPageData = function (a_config) {
    let data = {};
    data.cpu_loadavg = GetText(a_config.loadavg.path);
    data.cpu_thermal = GetThermal(a_config.cpu_thermal.path, a_config.cpu_thermaldiv);
    return data;
}

function GetThermal(a_filePath, a_thermalDiv) {
    try {
        const tempStr = np_fs.readFileSync(a_filePath, 'utf-8');
        if (isNaN(parseInt(tempStr)))
            return sc_status.CPU_Thermal.NoData;
        const temp = (parseInt(tempStr) / a_thermalDiv).toFixed(2);
        return GetThermalStatus(temp);
    }
    catch {
        return sc_status.CPU_Thermal.NotSupported;
    }
}

let g_cpu_thermal_status = sc_status.CPU_Thermal.None
function GetThermalStatus(a_value) {

    function CheckTemp(val, min, max) {
        const hysteresis = g_cpu_thermal_status != sc_status.CPU_Thermal.None ? 1 : 0;
        return val >= min + hysteresis && val <= max - hysteresis; // hysteresis
    }

    function TryStatus(val, min, max, status) {
        if (!CheckTemp(val, min, max))
            return false;
        g_cpu_thermal_status = status;
        return true;
    }

    const stats = [
        { status: sc_status.CPU_Thermal.Cold, t_min: -1000, t_max: 30 },
        { status: sc_status.CPU_Thermal.Normal, t_min: 30, t_max: 40 },
        { status: sc_status.CPU_Thermal.Warm, t_min: 40, t_max: 50 },
        { status: sc_status.CPU_Thermal.Hot, t_min: 50, t_max: 70 },
        { status: sc_status.CPU_Thermal.Critical, t_min: 70, t_max: 1000 },
    ];

    const temp = Math.floor(a_value);

    for (const i of stats) {
        if (TryStatus(temp, i.t_min, i.t_max, i.status))
            break;
    }

    let st = Object.assign({}, g_cpu_thermal_status);
    st.text = a_value + ' \xB0C'; // 0xB0 - celsius degree sign
    return st;
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