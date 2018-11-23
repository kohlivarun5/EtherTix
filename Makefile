release:
	-git branch -D gh-pages 
	git checkout -b gh-pages 
	npm install
	npm -f install react-responsive-carousel
	npm run clean
	npm run build
	npm run webpack:production
	git add -f img
	git add -f build/Index.js 
	git add -f node_modules/react-responsive-carousel/lib/styles/carousel.min.css
	git commit -am 'Add Build'
	git push -f https://github.com/EtherTix/ethertix.github.io.git gh-pages:master
