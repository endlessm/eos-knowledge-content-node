const Eknc = require('../index.js');

Eknc.start_glib_mainloop();
let engine = Eknc.Engine.get_default();
engine.default_app_id = 'com.endlessm.soccer.en';

let query = new Eknc.QueryObject({
    query: 'foot',
    limit: 10,
    tags_match_all: ['EknArticleObject'],
});
console.log('Trying a query');
engine.query(query).then(function (results) {
    console.log('Got results!');
    for (let result of results.get_models()) {
        console.log(result.constructor.name + ': ' + result.title);
        console.log(' ', result.tags);
        let { mime_type, data } = engine.get_domain().read_uri(result.ekn_id);
        if (data)
            console.log('  ' + mime_type + ': ' + data.toString('utf8', 0, 20) + '...');
    }
    console.log('Trying to fetch an object by id');
    return engine.get_object(results.get_models()[0].ekn_id).then(function (model) {
        console.log('Fetched a valid ekn id!')
        console.log(' ', model.ekn_id);
    });
}).catch(function (error) {
    console.log('Error :(');
    console.log(error);
}).then(function () {
    console.log('Trying to force an error');
    return engine.get_object('ekn:///0000000000000000000000000000000000000000').catch(function (error) {
        console.log('Error are working!');
    });
}).then(function () {
    console.log('All good');
    process.exit(0);
});
