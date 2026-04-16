var Clay = require("@rebble/clay");
var clayConfig = require("./config.json");
var clay = new Clay(clayConfig);

Pebble.addEventListener("ready", function () {
  console.log("PebbleKit JS ready!");
});
