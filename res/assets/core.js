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

function _jsonGet(jsonStr, key)
{
	if (jsonStr == "null")
		jsonStr = '{}';
	var v = Duktape.dec('jc', jsonStr);
	return (typeof key === 'undefined') ? v : v[key];
}

function toast(msg, duration)
{
	if (typeof duration === 'undefined')
		duration = 0;
	spawnNotification(msg, duration);
}
