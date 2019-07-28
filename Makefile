release:
	-git branch -D gh-pages 
	git checkout -b gh-pages 
	npm install
	npm run clean
	npm run build
	npm run webpack:production
	git add -f img
	git add -f build/Index.js 
	git commit -am 'Add Build'
	git push -f https://github.com/EtherTix/ethertix.github.io.git gh-pages:master
