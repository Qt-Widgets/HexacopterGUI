var map;
var polyline;

function initialize()
{
    // initialize map
    var myOptions =
    {
        center: new google.maps.LatLng(49.7780731,9.9430286),
        zoom: 5,
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
    polyPoints = new google.maps.MVCArray();
    initPolyline();
}

function initPolyline(){
    var polyOptions = {
        path: polyPoints,
        strokeColor: '#000000',
        strokeOpacity: 1.0,
        strokeWeight: 3};
    polyline = new google.maps.Polyline(polyOptions);

}

function setPath(latitude,longitude){
    planned.testValues(12212);
//    setPolyline(latitude,longitude);
//    polyline.setMap(map);
    map.setCenter(new google.maps.LatLng(latitude,longitude));

}



function setPolyline(latitude,longitude){
    polyPoints = polyline.getPath();
    polyPoints.push(new google.maps.LatLng(latitude, longitude));
}


function clear(){
    initPolyline();
    polyline.setMap(null);
    polyline.getPath().clear();

}

google.maps.event.addDomListener(window, 'load', initialize);
