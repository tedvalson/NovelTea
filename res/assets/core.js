if (typeof console === 'undefined')
{
	Object.defineProperty(this, 'console', {
		value: {}, writable: true, enumerable: false, configurable: true
	});
}

if (typeof console.log === 'undefined')
{
	(function () {
		var origPrint = print;  // capture in closure in case changed later
		Object.defineProperty(this.console, 'log', {
			value: function () {
				var strArgs = Array.prototype.map.call(arguments, function (v) { return String(v); });
				origPrint(Array.prototype.join.call(strArgs, ' '));
			}, writable: true, enumerable: false, configurable: true
		});
	})();
}

Math.seed = function(seed)
{
	Script.seed(seed);
}

Math.random = function()
{
	return Script.rand();
}

function _jsonGet(jsonStr, key)
{
	if (jsonStr == "null")
		jsonStr = '{}';
	var v = Duktape.dec('jc', jsonStr);
	return (typeof key === 'undefined') ? v : v[key];
}

function _propGet(jsonStr, key)
{
	var v = _jsonGet(jsonStr, key);
	return (typeof v === 'String') ? Script.evalExpressions(v) : v;
}

function toast(msg, addToLog, duration)
{
	if (typeof addToLog === 'undefined')
		addToLog = true;
	if (typeof duration === 'undefined')
		duration = 0;
	Game.spawnNotification(msg, addToLog, duration);
}

var _entity;
function prop(key, defaultValue)
{
	return _entity.prop(key, defaultValue);
}

function setProp(key, val)
{
	_entity.setProp(key, val);
}
