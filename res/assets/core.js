"use strict";

print("Loaded core.js");
print("--------------");

this.global = this;

var origPrint = print;  // capture in case changed later
this.console = {
	log : function(){
		var strArgs = Array.prototype.map.call(arguments, function (v) { return String(v); });
		origPrint(Array.prototype.join.call(strArgs, ' '));
	}
};
		
Math.seed = function(seed)
{
	Script.seed(seed);
}

Math.random = function()
{
	return Script.rand();
}

this._jsonGet = function(jsonStr, key)
{
	if (jsonStr == "null")
		jsonStr = '{}';
	var v = Duktape.dec('jc', jsonStr);
	return (typeof key === 'undefined') ? v : v[key];
}

this._propGet = function(jsonStr, key)
{
	var v = _jsonGet(jsonStr, key);
	return (typeof v === 'String') ? Script.evalExpressions(v) : v;
};

this.toast = function(msg, addToLog, duration)
{
	if (typeof addToLog === 'undefined')
		addToLog = true;
	if (typeof duration === 'undefined')
		duration = 0;
	Game.spawnNotification(msg, addToLog, duration);
};

this.thisEntity = 0;

this._setActiveEntity = function(e)
{
	thisEntity = e;
}

this.prop = function(key, defaultValue)
{
	return thisEntity.prop(key, defaultValue);
}

this.setProp = function(key, val)
{
	thisEntity.setProp(key, val);
}
