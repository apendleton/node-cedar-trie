"use strict";

var argv = require('minimist')(process.argv.slice(2));
if (!argv.iterations || !argv.concurrency) {
  console.error('Please provide desired iterations, concurrency');
  console.error('Example: \nnode bench/hello_object_async.bench.js --iterations 50 --concurrency 10');
  process.exit(1);
}

// This env var sets the libuv threadpool size.
// This value is locked in once a function interacts with the threadpool
// Therefore we need to set this value either in the shell or at the very
// top of a JS file (like we do here)
process.env.UV_THREADPOOL_SIZE = argv.concurrency;

var fs = require('fs');
var path = require('path');
var assert = require('assert')
var d3_queue = require('d3-queue');
var module = require('../lib/index.js');

var H = new module.HelloObjectAsync('park bench');
var queue = d3_queue.queue();
var runs = 0;

function run(cb) {
    H.helloAsync({ louder: false }, function(err, result) {
      if (err) {
        return cb(err);
      }
      ++runs;
      return cb();
    });
}

// Start monitoring time before async work begins within the defer iterator below.
// AsyncWorkers will kick off actual work before the defer iterator is finished, 
// and we want to make sure we capture the time of the work of that initial cycle.
var time = +(new Date());

for (var i = 0; i < argv.iterations; i++) {
    queue.defer(run);
}

queue.awaitAll(function(error) {
  if (error) throw error;
  if (runs != argv.iterations) {
    throw new Error("Error: did not run as expected");
  }
  // check rate
  time = +(new Date()) - time;

  if (time == 0) {
    console.log("Warning: ms timer not high enough resolution to reliably track rate. Try more iterations");
  } else {
  // number of milliseconds per iteration
    var rate = runs/(time/1000);
    console.log('Benchmark speed: ' + rate.toFixed(0) + ' runs/s (runs:' + runs + ' ms:' + time + ' )');
  }

  console.log("Benchmark iterations:",argv.iterations,"concurrency:",argv.concurrency)

  // There may be instances when you want to assert some performance metric
  //assert.equal(rate > 1000, true, 'speed not at least 1000/second ( rate was ' + rate + ' runs/s )');

});