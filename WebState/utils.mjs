'use strict';
export function distance(a, b) { return Math.sqrt((b.x - a.x) ** 2 + (b.y - a.y) ** 2); } 

//exports.StatusColor = {
//    None: 'gray',
//    Cold: 'blue', Normal: 'green', Warm: 'yellow', Hot: 'orange', Critical: 'red', // Temperature
//    Valid: 'green', Invalid: ''
//}


//if (CheckColour(a_value, 0, 30, 'blue'))
//    return g_cpu_thermal_status;
//if (CheckColour(a_value, 30, 40, 'green'))
//    return g_cpu_thermal_status;
//if (CheckColour(a_value, 40, 50, 'yellow'))
//    return g_cpu_thermal_status;
//if (CheckColour(a_value, 50, 70, 'orange'))
//    return g_cpu_thermal_status;
//if (CheckColour(a_value, 70, 100, 'red'))
