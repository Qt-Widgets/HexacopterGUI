var map;
var geocoder;
var start;
var markers;

function initialize() {

    var myOptions =
    {
        center: new google.maps.LatLng(49.7913044, 9.9533548),
        zoom: 2,
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);



    google.maps.event.addListener(map,'click',function(event){addMarker(event);
                                                              gpscontrol.getMapCoordinates(event.latLng.lat(),event.latLng.lng());
                                                              gpscontrol.showClickedMarker(event.latLng.lat(),event.latLng.lng());
                                                          });
     geocoder = new google.maps.Geocoder();
     start=0;
     markers=new Array();
}


function addMarker(event){
    var coords = event.latLng;
    var marker = new google.maps.Marker({
        position: new google.maps.LatLng(coords.lat(),coords.lng()),
        map: map,
    });
    marker.setMap(map);
    markers.push(marker);
    google.maps.event.addListener(marker, "rightclick", function (){ marker.setMap(null); var position =marker.getPosition();gpscontrol.deleteMapCoordinates(position.lat(),position.lng());});
    google.maps.event.addListener(marker,'click',function(){gpscontrol.showClickedMarker(marker.getPosition().lat(),marker.getPosition().lng());});
}

function setCenter(latitude,longitude)
{
    map.setCenter(new google.maps.LatLng(latitude, longitude));
}

google.maps.event.addDomListener(window, 'load', initialize);

function setStart(latitude,longitude){
    if(start!=0){
        start.setMap(null);
    }
    setCenter(latitude,longitude);
    map.setZoom(12);

}
function setMarker(latitude,longitude){
    var marker = new google.maps.Marker({
                position: new google.maps.LatLng(latitude, longitude),
                map: map,
             });
        markers.push(marker);
}

function setPath(latitude,longitude){
    setMarker(latitude,longitude);
    markers.setMap(map);
}

function setStartExact(latitude,longitude){
    if(start!=0){
        start.setMap(null);
    }
    setCenter(latitude,longitude);
    map.setZoom(17);
    var marker = new google.maps.Marker({
        position: new google.maps.LatLng(latitude,longitude),
        map: map,
    });
    marker.setMap(map);
    start=marker;
}


function codeAddress(address) {
  geocoder.geocode( { 'address': address}, function(results, status) {
    if (status == google.maps.GeocoderStatus.OK) {
      map.setCenter(results[0].geometry.location);
      map.setZoom(12);
    } else {
      alert('Geocode was not successful for the following reason: ' + status);
    }
  });
}

function clear(){
    start=0;
    for(var i=0;i<markers.length;i++){
        markers[i].setMap(null);
    }
    markers.length=0;
    gpscontrol.controlAdress(markers.length);
}
