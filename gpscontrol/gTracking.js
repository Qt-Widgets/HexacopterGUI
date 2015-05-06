var map;
var coords;
var polyline;
var first;
var startMarker;
var currentMarker;
//var polyPoints;

function initialize()
{
    // initialize map
    var myOptions =
            {
        center: new google.maps.LatLng(49.7780731,9.9430286),
        zoom: 15,
        mapTypeId: google.maps.MapTypeId.SATELLITE,
    };

    map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
    polyPoints = new google.maps.MVCArray();
    initPolyline();

    first = 1;
}

function initPolyline(){
    var polyOptions = {
        path: polyPoints,
        strokeColor: '#FF6262',
        strokeOpacity: 1.0,
        strokeWeight: 3};
    polyline = new google.maps.Polyline(polyOptions);
    polyline.setMap(map);
}

function setCenter(){
    map.setCenter(currentMarker.getPosition());
}

function setPath(latitude,longitude){
    setMarker(latitude,longitude);
    setPolyline(latitude,longitude);
}

function setMarker(latitude,longitude){
    if (first == 1){
        first = 0;
        startMarker = new google.maps.Marker(
                    {
                        icon: 'http://maps.google.com/mapfiles/ms/icons/green-dot.png',
                        position: new google.maps.LatLng(latitude, longitude),
                        map: map,
                    });
        currentMarker = new google.maps.Marker(
                    {
                        icon: 'http://maps.google.com/mapfiles/ms/icons/red-dot.png',
                        position: new google.maps.LatLng(latitude, longitude),
                        map: map,
                    });
        startMarker.setMap(map);
        currentMarker.setMap(map);

    } else {
        currentMarker.setIcon('http://maps.google.com/mapfiles/ms/icons/red-dot.png');
        currentMarker.setPosition(new google.maps.LatLng(latitude, longitude))
    }

}

function setPolyline(latitude,longitude){
    polyPoints = polyline.getPath();
    polyPoints.push(new google.maps.LatLng(latitude, longitude));
}


function clear(){
    startMarker.setMap(null);
    currentMarker.setMap(null);
    polyline.setMap(null);
    polyline.getPath().clear();
    initPolyline();
    first = 1;
}

google.maps.event.addDomListener(window, 'load', initialize);
