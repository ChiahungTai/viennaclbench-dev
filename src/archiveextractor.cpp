#include "archiveextractor.h"

/*
 * ArchiveExtractor class provides easy decompression of .tar.gz files
 * use the function extractFile(QString filePath) to perform the extraction
 * can be easily modified to provide support for additional formats
 * */

ArchiveExtractor::ArchiveExtractor(QObject *parent) :
  QObject(parent)
{
}

void ArchiveExtractor::extractFile(QString filePath)
{
  struct archive *a;
  struct archive *ext;
  struct archive_entry *entry;
  int r;
  //  /* The "flags" argument selects optional behavior, 'OR' the flags you want. */
  //  /* Default: Do not try to set owner/group. */
  //#define	ARCHIVE_EXTRACT_OWNER			(0x0001)
  //  /* Default: Do obey umask, do not restore SUID/SGID/SVTX bits. */
  //#define	ARCHIVE_EXTRACT_PERM			(0x0002)
  //  /* Default: Do not restore mtime/atime. */
  //#define	ARCHIVE_EXTRACT_TIME			(0x0004)
  //  /* Default: Replace existing files. */
  //#define	ARCHIVE_EXTRACT_NO_OVERWRITE 		(0x0008)
  //  /* Default: Try create first, unlink only if create fails with EEXIST. */
  //#define	ARCHIVE_EXTRACT_UNLINK			(0x0010)
  //  /* Default: Do not restore ACLs. */
  //#define	ARCHIVE_EXTRACT_ACL			(0x0020)
  //  /* Default: Do not restore fflags. */
  //#define	ARCHIVE_EXTRACT_FFLAGS			(0x0040)
  //  /* Default: Do not restore xattrs. */
  //#define	ARCHIVE_EXTRACT_XATTR 			(0x0080)
  //  /* Default: Do not try to guard against extracts redirected by symlinks. */
  //  /* Note: With ARCHIVE_EXTRACT_UNLINK, will remove any intermediate symlink. */
  //#define	ARCHIVE_EXTRACT_SECURE_SYMLINKS		(0x0100)
  //  /* Default: Do not reject entries with '..' as path elements. */
  //#define	ARCHIVE_EXTRACT_SECURE_NODOTDOT		(0x0200)
  //  /* Default: Create parent directories as needed. */
  //#define	ARCHIVE_EXTRACT_NO_AUTODIR		(0x0400)
  //  /* Default: Overwrite files, even if one on disk is newer. */
  //#define	ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER	(0x0800)
  //  /* Detect blocks of 0 and write holes instead. */
  //#define	ARCHIVE_EXTRACT_SPARSE			(0x1000)
  //  /* Default: Do not restore Mac extended metadata. */
  //  /* This has no effect except on Mac OS. */
  //#define	ARCHIVE_EXTRACT_MAC_METADATA		(0x2000)
  int flags = 0;
  //      flags |= ARCHIVE_EXTRACT_TIME;
  //      flags |= ARCHIVE_EXTRACT_NO_AUTODIR;
  //      flags |= ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER;

  const char *filename = filePath.toUtf8().constData();

  //toggle extraction
  bool do_extract = true;

  a = archive_read_new();
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);

  //tuned for .tar.gz
  archive_read_support_filter_gzip(a);
  archive_read_support_format_gnutar(a);

  if((r = archive_read_open_filename(a, filename, 10240)) ){
    errmsg(archive_error_string(a));
  }
  for(;;){
    r = archive_read_next_header(a, &entry);
    if(r == ARCHIVE_EOF) break;
    if(r != ARCHIVE_OK){
      errmsg(archive_error_string(a));
    }
    if (verbose || !do_extract){
      std::cout << "Detected files in archive: ";
      msg(archive_entry_pathname(entry));
      std::cout << std::endl;
    }
    if (verbose && do_extract){
      msg("About to start extracting\n");
    }
    if (do_extract){
      std::cout << "Extracting" << std::endl;
      r = archive_write_header(ext, entry);
      if (r != ARCHIVE_OK) errmsg(archive_error_string(a));
      else copy_data(a, ext);
      std::cout << "File extracted: ";
      msg(archive_entry_pathname(entry));
      std::cout << std::endl;
    }
  }
  archive_read_close(a);
  archive_read_free(a);
}

int ArchiveExtractor::copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
  int64_t offset;
#else
  off_t offset;
#endif

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r != ARCHIVE_OK)
      return (r);
    r = archive_write_data_block(aw, buff, size, offset);
    if (r != ARCHIVE_OK) {
      warn("archive_write_data_block()",
           archive_error_string(aw));
      return (r);
    }
  }
}

/*
 * Reporting functions
 * */

void ArchiveExtractor::msg(const char *m)
{
  std::cout << m;
}

void ArchiveExtractor::errmsg(const char *m)
{
  std::cout << "Error: " << m;
}

void ArchiveExtractor::warn(const char *f, const char *m)
{
  errmsg(f);
  errmsg(" failed: ");
  errmsg(m);
  errmsg("\n");
}