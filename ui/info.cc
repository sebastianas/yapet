// $Id$
//
// Copyright (C) 2008-2013  Rafael Ostertag
//
// This file is part of YAPET.
//
// YAPET is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// YAPET is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// YAPET.  If not, see <http://www.gnu.org/licenses/>.
//

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_CRYPTO_H
# include <openssl/crypto.h>
#endif

#include <sys/resource.h>

#include <sstream>

#include "intl.h"
#include "globals.h"
#include "info.h"

//
// Private
//

//
// Protected
//

//
// Public
//
InfoDialog::InfoDialog(YACURS::ListBox<>::lsz_t entries):
    YACURS::Dialog(_("Information"), YACURS::OK_ONLY, YACURS::AUTOMATIC ),
    mainpack(),
    leftpack(),
    rightpack(),
    fn(_("File: ")),
    fn_status(std::string()),
    fp(_("File permission check: ")),
	slt(_("Screen lock time-out: ")),
	slt_status(),
	sult(_("Unlock Dialog time-out: ")),
	sult_status(),
	ent(_("Password records: ")),
	ent_status(),
	mod(_("Records modified: ")),
	mod_status(),
	ver(_("PET file version: ")),
	ver_status(),
	pwset(_("Password set: ")),
	pwset_status(),
	arch(_("Architecture: ")),
	arch_status(),
	ssl("OpenSSL:"),
#if defined(HAVE_SSLEAY_VERSION)
    ssl_status(SSLeay_version(SSLEAY_VERSION)),
#else
    ssl_status(_("n/a")),
#endif
    ttl(_("Terminal Title:")),
#if defined(ENABLE_TERMINALTITLE)
    ttl_status(_("built-in")),
#else
    ttl_status(_("not built-in")),
#endif
    pwg(_("Password Generator:")),
#if defined(ENABLE_PWGEN)
    pwg_status(_("built-in")),
#else
    pwg_status(_("not built-in")),
#endif
    cfs(_("Core File:")),
#if defined(RLIMIT_CORE) && !defined(DEBUG)
    cfs_status(_("suppressed"))
#else
    cfs_status(_("not suppressed"))
#endif
 {

    // used to make the file name label get the entire space available
    // to the pack.
    rightpack.always_dynamic(true);
    rightpack.hinting(false);

    mainpack.add_back(&leftpack);
    mainpack.add_back(&rightpack);
    widget(&mainpack);

    leftpack.add_back(&fn);
    if (YAPET::Globals::file!=0)
	fn_status.label(YAPET::Globals::file->getFilename());
    else
	fn_status.label(_("none loaded"));
    rightpack.add_back(&fn_status);

    leftpack.add_back(&fp);
    if (YAPET::Globals::config.filesecurity)
	fp_status.label(_("enabled"));
    else
	fp_status.label(_("disabled"));
    rightpack.add_back(&fp_status);

    leftpack.add_back(&slt);
    std::ostringstream val;
    val << YAPET::Globals::config.timeout;
    slt_status.label(val.str());
    rightpack.add_back(&slt_status);

    leftpack.add_back(&sult);
    val.str("");
    val.clear();
    val << YAPET::Globals::config.pw_input_timeout;
    sult_status.label(val.str());
    rightpack.add_back(&sult_status);

    leftpack.add_back(&ent);
    val.str("");
    val.clear();
    val << entries;
    ent_status.label(val.str());
    rightpack.add_back(&ent_status);

    leftpack.add_back(&mod);
    if (YAPET::Globals::records_changed)
	mod_status.label(_("yes"));
    else
	mod_status.label(_("no"));
    rightpack.add_back(&mod_status);

    leftpack.add_back(&ver);
    if (YAPET::Globals::file != 0 && YAPET::Globals::key != 0) {
	val.str("");
	val.clear();
	val << YAPET::Globals::file->getFileVersion(*YAPET::Globals::key);
	ver_status.label(val.str());
    } else {
	ver_status.label(_("n/a"));
    }
    rightpack.add_back(&ver_status);

    leftpack.add_back(&pwset);
    if (YAPET::Globals::file != 0 && YAPET::Globals::key != 0) {
	time_t t= static_cast<time_t>(YAPET::Globals::file->getMasterPWSet (*YAPET::Globals::key));
	pwset_status.label(asctime(localtime(&t)));
    } else {
	pwset_status.label(_("n/a"));
    }
    rightpack.add_back(&pwset_status);

    leftpack.add_back(&arch);
    std::string archstr;
#if defined(SIZEOF_INT_P)
    val.str("");
    val.clear();
    val << 8 * SIZEOF_INT_P;
    archstr = val.str();
    archstr += "bit ";
#endif
#if defined(WORDS_BIGENDIAN)
    archstr += _("big endian");
#else
    archstr += _("little endian");
#endif
    arch_status.label(archstr);
    rightpack.add_back(&arch_status);

    leftpack.add_back(&ssl);
    rightpack.add_back(&ssl_status);

    leftpack.add_back(&ttl);
    rightpack.add_back(&ttl_status);

    leftpack.add_back(&pwg);
    rightpack.add_back(&pwg_status);

    leftpack.add_back(&cfs);
    rightpack.add_back(&cfs_status);

    // colors
    fn.color(YACURS::DIALOG);
    fn_status.color(YACURS::DIALOG);

    fp.color(YACURS::DIALOG);
    fp_status.color(YACURS::DIALOG);

    slt.color(YACURS::DIALOG);
    slt_status.color(YACURS::DIALOG);

    sult.color(YACURS::DIALOG);
    sult_status.color(YACURS::DIALOG);

    ent.color(YACURS::DIALOG);
    ent_status.color(YACURS::DIALOG);

    mod.color(YACURS::DIALOG);
    mod_status.color(YACURS::DIALOG);

    ver.color(YACURS::DIALOG);
    ver_status.color(YACURS::DIALOG);

    pwset.color(YACURS::DIALOG);
    pwset_status.color(YACURS::DIALOG);

    arch.color(YACURS::DIALOG);
    arch_status.color(YACURS::DIALOG);

    ssl.color(YACURS::DIALOG);
    ssl_status.color(YACURS::DIALOG);

    ttl.color(YACURS::DIALOG);
    ttl_status.color(YACURS::DIALOG);

    pwg.color(YACURS::DIALOG);
    pwg_status.color(YACURS::DIALOG);

    cfs.color(YACURS::DIALOG);
    cfs_status.color(YACURS::DIALOG);
}

InfoDialog::~InfoDialog() {

}
