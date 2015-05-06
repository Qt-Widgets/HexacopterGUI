var map;
var markers;
var points;
var lineLayer;
var style;
var line;
var coords;
var lineFeature;

function initialize()
{
    map = new OpenLayers.Map("mapdiv");
    map.addLayer(new OpenLayers.Layer.OSM());

    map.setCenter(new OpenLayers.LonLat(9.9533548,49.7913044).transform(
                               new OpenLayers.Projection("EPSG:4326"),
                                           map.getProjectionObject()),18);

    markers = new OpenLayers.Layer.Markers( "Markers" );
    style = { strokeColor: '#000000', strokeOpacity: 1.5,strokeWidth: 3};

    lineLayer = new OpenLayers.Layer.Vector("Line Layer");
    map.addLayer(lineLayer);
    map.addControl(new OpenLayers.Control.DrawFeature(lineLayer, OpenLayers.Handler.Path));
    line = new OpenLayers.Geometry.LineString();
    points = [];
    coords = new Array();
}

function setCenter(){
    map.setCenter(coords[coords.length-1], 18);
}

function setPath(latitude,longitude){
    setMarker(latitude, longitude);
    setPolyline(latitude,longitude);
    coords.push(new OpenLayers.LonLat(longitude,latitude).transform(
            new OpenLayers.Projection("EPSG:4326"), map.getProjectionObject()));
}

function display(option){
    switch(option){
        case 0:
                map.addLayer(lineLayer);
                map.removeLayer(markers);
                break;
        case 1:
                map.addLayer(lineLayer);
                map.addLayer(markers);
                break;

    }
}

function setMarker(latitude, longitude){
var lonLat = new OpenLayers.LonLat(longitude,latitude).transform(
                          new OpenLayers.Projection("EPSG:4326"),
                                      map.getProjectionObject() );
markers.addMarker(new OpenLayers.Marker(lonLat));
}

function setPolyline(latitude,longitude){
    line.addPoint(new OpenLayers.Geometry.Point(longitude, latitude).transform(new OpenLayers.Projection("EPSG:4326"), map.getProjectionObject()));
    lineFeature = new OpenLayers.Feature.Vector(line, null, style);
    lineLayer.addFeatures([lineFeature]);
}

function removeMarker(){
    markers.removeMarker(markers.marker[0]);
}

function clear(){
    map.removeLayer(lineLayer);
    markers.clearMarkers();
    lineLayer = new OpenLayers.Layer.Vector("Line Layer");
    line = new OpenLayers.Geometry.LineString();
    lineFeature = new OpenLayers.Feature.Vector(line, null, style);
}
