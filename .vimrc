func! WordProcessorMode() 
  setlocal formatoptions=1 
  setlocal noexpandtab 
  map j gj 
  map k gk
  setlocal spell spelllang=en_us 
"  set thesaurus+=/Users/sbrown/.vim/thesaurus/mthesaur.txt
  set complete+=s
  set formatprg=par
  setlocal wrap 
  setlocal linebreak 
endfu 
com! WP call WordProcessorMode()

"let g:loaded_youcompleteme = 1

let mapleader = ","
map <leader>pm :set paste<return>
map <leader>n :lnext<return>
map <leader>p :lprev<return>
map <leader>tn :tabnext<return>
map <leader>tp :tabprevious<return>
map <leader>cr :s/\(\w*\)/\1::\1/<return>:noh<return>0v2f:y0
map <leader>gu i#ifndef<return>#define<return>#endif //<return><esc>gg 

set background=dark
"colorscheme vimbrant
highlight ColorColumn ctermbg=7
highlight ColorColumn guibg=Gray
set number
set hlsearch

set nocompatible              " be iMproved, required
filetype off                  " required

syntax enable
au BufRead,BufNewFile *.ebnf set filetype=ebnf
au BufRead,BufNewFile *.glsl set filetype=glsl

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'
Plugin 'https://github.com/Valloric/YouCompleteMe.git'
Plugin 'https://github.com/tikhomirov/vim-glsl'
let g:ycm_confirm_extra_conf = 0
" ycm turns errors into a list, allowing for jumping between errors
let g:ycm_always_populate_location_list = 1
Plugin 'octol/vim-cpp-enhanced-highlight'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line

"tab width
set tabstop=4     
set shiftwidth=4    
set softtabstop=4  
set expandtab    

set nobackup
set noswapfile
